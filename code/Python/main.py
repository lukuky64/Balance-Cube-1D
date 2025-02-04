import pybullet as p
import pybullet_data
import LQReg
import time
import math

# Connect to simulation and reset
p.connect(p.GUI)

p.resetDebugVisualizerCamera(cameraDistance=0.5,  # smaller distance for a closer view
                             cameraYaw=50,         # adjust as needed
                             cameraPitch=-35,      # adjust as needed
                             cameraTargetPosition=[0, 0, 0])


p.resetSimulation()
p.setAdditionalSearchPath(pybullet_data.getDataPath())
p.setGravity(0, 0, -9.81)

# Load a ground plane for realistic contact
plane = p.loadURDF("plane.urdf")

# --- Define the rectangular prism parameters ---
# Let the square side length be 'side' and the extrusion (thickness) be 1/4 of side.
side = 0.2          # side length of the square
thickness = side/4  # extrusion thickness
# For a box, the half extents are half the dimensions:
half_x = thickness/2
half_y = side/2
half_z = side/2

# Create collision and visual shapes for the base (the prism)
base_collision = p.createCollisionShape(p.GEOM_BOX, halfExtents=[half_x, half_y, half_z])
base_visual = p.createVisualShape(p.GEOM_BOX, halfExtents=[half_x, half_y, half_z], rgbaColor=[1, 0, 0, 1])

# --- Set the initial orientation so that the prism balances on an edge ---
# Here we rotate about the x-axis by 45°.
roll = 1.05*(math.pi/4)   # 45° rotation about x
pitch = 0
yaw = 0
base_orientation = p.getQuaternionFromEuler([roll, pitch, yaw])

# Calculate the minimum z-coordinate of the rotated box to ensure the edge contacts the ground.
min_z = (-half_y * math.sin(roll)) + (-half_z * math.cos(roll))
# Offset the base upward so that the minimum z equals 0.
base_z_offset = -min_z
base_position = [0, 0, base_z_offset]

# --- Define the reaction wheel parameters ---
wheel_mass = 0.1
wheel_radius = 0.09
wheel_height = 0.06
# We want the wheel to rotate about the base's local x-axis.
# Rotate the cylinder by 90° about y so that its long axis aligns with the x-axis.
wheel_orientation = p.getQuaternionFromEuler([0, math.pi/2, 0])
wheel_collision = p.createCollisionShape(p.GEOM_CYLINDER,
                                         radius=wheel_radius,
                                         height=wheel_height,
                                         collisionFrameOrientation=wheel_orientation)
wheel_visual = p.createVisualShape(p.GEOM_CYLINDER,
                                   radius=wheel_radius,
                                   length=wheel_height,
                                   visualFrameOrientation=wheel_orientation,
                                   rgbaColor=[0, 1, 0, 1])

# --- Build the multi-body: base (prism) with an attached reaction wheel ---
# The reaction wheel is attached at the centre of the base.
linkMasses = [wheel_mass]
linkCollisionShapeIndices = [wheel_collision]
linkVisualShapeIndices = [wheel_visual]
linkPositions = [[0, 0, 0]]                   # attach at the base's centre
linkOrientations = [[0, 0, 0, 1]]             # no extra rotation relative to base
linkInertialFramePositions = [[0, 0, 0]]
linkInertialFrameOrientations = [[0, 0, 0, 1]]
linkParentIndices = [0]
linkJointTypes = [p.JOINT_REVOLUTE]
# Joint axis along the base's local x-axis.
linkJointAxis = [[1, 0, 0]]

robot = p.createMultiBody(baseMass=1.0,
                          baseCollisionShapeIndex=base_collision,
                          baseVisualShapeIndex=base_visual,
                          basePosition=base_position,
                          baseOrientation=base_orientation,
                          linkMasses=linkMasses,
                          linkCollisionShapeIndices=linkCollisionShapeIndices,
                          linkVisualShapeIndices=linkVisualShapeIndices,
                          linkPositions=linkPositions,
                          linkOrientations=linkOrientations,
                          linkInertialFramePositions=linkInertialFramePositions,
                          linkInertialFrameOrientations=linkInertialFrameOrientations,
                          linkParentIndices=linkParentIndices,
                          linkJointTypes=linkJointTypes,
                          linkJointAxis=linkJointAxis)

# Disable the default motor control on the reaction wheel joint.
p.setJointMotorControl2(bodyUniqueId=robot,
                        jointIndex=0,
                        controlMode=p.VELOCITY_CONTROL,
                        force=0)

K_1_init = -1.2
K_2_init = -0.3
K_3_init = 0
K_4_init = -0.001

# --- Create an instance of the LQReg class ---
lqr = LQReg.LQReg(K_1_init, K_2_init, K_3_init, K_4_init)

K1_slider = p.addUserDebugParameter("K1", -3, 3, K_1_init)
K2_slider = p.addUserDebugParameter("K2", -3, 3, K_2_init)
K3_slider = p.addUserDebugParameter("K3", -3, 3, K_3_init)
K4_slider = p.addUserDebugParameter("K4", -3, 3, K_4_init)


# We'll use a manually stepped simulation
p.setRealTimeSimulation(0)

# --- Main simulation loop ---
while p.isConnected():
    # --- Compute the state ---
    # Get the base (cube) position and orientation.
    basePos, baseOrn = p.getBasePositionAndOrientation(robot)
    # Convert quaternion to Euler angles.
    cubeEuler = p.getEulerFromQuaternion(baseOrn)
    # Define theta as the deviation of the cube's roll angle from the balanced position (pi/4).
    theta = cubeEuler[0] - (math.pi/4)
    
    # Get the base's angular velocity.
    _, baseAngVel = p.getBaseVelocity(robot)
    # Assume the relevant angular velocity is about the x-axis.
    theta_dot = baseAngVel[0]
    
    # Get the reaction wheel joint state: angle and angular velocity.
    jointState = p.getJointState(robot, 0)
    phi = jointState[0]
    phi_dot = jointState[1]

    # get K values from sliders
    K_1 = p.readUserDebugParameter(K1_slider)
    K_2 = p.readUserDebugParameter(K2_slider)
    K_3 = p.readUserDebugParameter(K3_slider)
    K_4 = p.readUserDebugParameter(K4_slider)
    lqr.setGains(K_1, K_2, K_3, K_4)
    
    # Combine into the state vector.
    state = [theta, theta_dot, phi, phi_dot]
    # --- Compute and apply the control torque using LQR ---
    target_torque = lqr.computeLQR(state)
    
    p.setJointMotorControl2(bodyUniqueId=robot,
                            jointIndex=0,
                            controlMode=p.TORQUE_CONTROL,
                            force=target_torque)
    
    # Step the simulation
    p.stepSimulation()
    time.sleep(1./5000.)