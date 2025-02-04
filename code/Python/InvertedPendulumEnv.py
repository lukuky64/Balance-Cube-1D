import gym
from gym import spaces
import numpy as np
import pybullet as p
import pybullet_data
import math
import time

class InvertedPendulumEnv(gym.Env):
    """
    A custom Gym environment for the rectangular prism (with a reaction wheel)
    balancing on an edge. The state is [theta, theta_dot, phi, phi_dot],
    where theta is the deviation (roll minus π/4) of the prism.
    The action is the torque applied to the reaction wheel.
    """
    metadata = {'render.modes': ['human']}
    
    def __init__(self, render=True):
        super(InvertedPendulumEnv, self).__init__()
        self.render_mode = render
        
        # Connect to PyBullet
        if self.render_mode:
            self.physics_client = p.connect(p.GUI)
            # Optionally set a custom camera
            p.resetDebugVisualizerCamera(cameraDistance=0.5,
                                         cameraYaw=50,
                                         cameraPitch=-35,
                                         cameraTargetPosition=[0, 0, 0])
        else:
            self.physics_client = p.connect(p.DIRECT)
        
        p.setAdditionalSearchPath(pybullet_data.getDataPath())
        p.setGravity(0, 0, -9.81)
        
        # Define action space: a continuous torque value.
        self.max_torque = 10.0
        self.action_space = spaces.Box(low=-self.max_torque, 
                                       high=self.max_torque, 
                                       shape=(1,),
                                       dtype=np.float32)
        
        # Define observation space: [theta, theta_dot, phi, phi_dot]
        # Assume approximate bounds:
        obs_low = np.array([-math.pi/4, -10, -math.pi, -10], dtype=np.float32)
        obs_high = np.array([ math.pi/4,  10,  math.pi,  10], dtype=np.float32)
        self.observation_space = spaces.Box(low=obs_low, high=obs_high, dtype=np.float32)
        
        self.timeStep = 1/240.0  # simulation time step
        p.setTimeStep(self.timeStep)
        
        self._setup_simulation()
    
    def _setup_simulation(self):
        # Reset simulation and load the ground plane.
        p.resetSimulation()
        p.setAdditionalSearchPath(pybullet_data.getDataPath())
        p.setGravity(0, 0, -9.81)
        self.plane = p.loadURDF("plane.urdf")
        
        # --- Define the rectangular prism (base) parameters ---
        side = 0.2           # square side length
        thickness = side/4   # extrusion thickness
        # Define box half-extents: here we assume the prism is extruded along the x-axis.
        half_x = thickness/2
        half_y = side/2
        half_z = side/2
        
        base_collision = p.createCollisionShape(p.GEOM_BOX, halfExtents=[half_x, half_y, half_z])
        base_visual = p.createVisualShape(p.GEOM_BOX, halfExtents=[half_x, half_y, half_z], rgbaColor=[1, 0, 0, 1])
        
        # --- Set the initial orientation so the prism balances on an edge ---
        # In the balanced configuration, the roll angle should be π/4.
        roll = math.pi/4  
        pitch = 0
        yaw = 0
        base_orientation = p.getQuaternionFromEuler([roll, pitch, yaw])
        
        # Calculate the vertical offset so that the prism's lower edge touches the ground.
        min_z = (-half_y * math.sin(roll)) + (-half_z * math.cos(roll))
        base_z_offset = -min_z
        base_position = [0, 0, base_z_offset]
        
        # --- Define the reaction wheel parameters ---
        wheel_mass = 0.1
        wheel_radius = 0.09
        wheel_height = 0.06
        # To have the wheel rotate about the base's local x-axis, rotate the cylinder
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
        
        # --- Build the multi-body (prism with attached reaction wheel) ---
        linkMasses = [wheel_mass]
        linkCollisionShapeIndices = [wheel_collision]
        linkVisualShapeIndices = [wheel_visual]
        linkPositions = [[0, 0, 0]]  # attach at the center of the base
        linkOrientations = [[0, 0, 0, 1]]
        linkInertialFramePositions = [[0, 0, 0]]
        linkInertialFrameOrientations = [[0, 0, 0, 1]]
        linkParentIndices = [0]
        linkJointTypes = [p.JOINT_REVOLUTE]
        linkJointAxis = [[1, 0, 0]]  # joint axis along the base's local x-axis
        
        self.robot = p.createMultiBody(baseMass=1.0,
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
        
        # Disable default motor control on the reaction wheel joint.
        p.setJointMotorControl2(bodyUniqueId=self.robot,
                                jointIndex=0,
                                controlMode=p.VELOCITY_CONTROL,
                                force=0)
    
    def step(self, action):
        # Action is the torque command for the reaction wheel.
        target_torque = float(action[0])
        p.setJointMotorControl2(bodyUniqueId=self.robot,
                                jointIndex=0,
                                controlMode=p.TORQUE_CONTROL,
                                force=target_torque)
        
        # Advance the simulation for a fixed number of steps.
        # (This lets one RL step correspond to a larger time interval.)
        for _ in range(10):
            p.stepSimulation()
            if self.render_mode:
                time.sleep(self.timeStep)
        
        # --- Get the state ---
        basePos, baseOrn = p.getBasePositionAndOrientation(self.robot)
        cubeEuler = p.getEulerFromQuaternion(baseOrn)
        # theta: deviation from the balanced state (roll - π/4)
        theta = cubeEuler[0] - (math.pi/4)
        _, baseAngVel = p.getBaseVelocity(self.robot)
        theta_dot = baseAngVel[0]
        jointState = p.getJointState(self.robot, 0)
        phi = jointState[0]
        phi_dot = jointState[1]
        state = np.array([theta, theta_dot, phi, phi_dot], dtype=np.float32)
        
        # --- Define a reward ---
        # Here, we penalise deviation from zero and optionally the magnitude of the torque.
        reward = - (abs(theta) + 0.1*abs(theta_dot) + 0.01*abs(target_torque))
        
        # --- Define termination condition ---
        # For example, end the episode if the prism tilts too far.
        done = bool(abs(theta) > (math.pi/8))
        
        # Additional info (if needed)
        info = {}
        
        return state, reward, done, info
    
    def reset(self):
        # Reset simulation and return the initial state.
        self._setup_simulation()
        # Let the simulation settle for a few steps.
        for _ in range(10):
            p.stepSimulation()
            if self.render_mode:
                time.sleep(self.timeStep)
                
        basePos, baseOrn = p.getBasePositionAndOrientation(self.robot)
        cubeEuler = p.getEulerFromQuaternion(baseOrn)
        theta = cubeEuler[0] - (math.pi/4)
        _, baseAngVel = p.getBaseVelocity(self.robot)
        theta_dot = baseAngVel[0]
        jointState = p.getJointState(self.robot, 0)
        phi = jointState[0]
        phi_dot = jointState[1]
        state = np.array([theta, theta_dot, phi, phi_dot], dtype=np.float32)
        return state
    
    def render(self, mode='human'):
        # Rendering is handled by PyBullet's GUI.
        pass
    
    def close(self):
        p.disconnect()

# Example usage:
if __name__ == '__main__':
    env = InvertedPendulumEnv(render=True)
    obs = env.reset()
    for _ in range(1000):
        # For testing, take random actions.
        action = env.action_space.sample()
        obs, reward, done, info = env.step(action)
        if done:
            obs = env.reset()
    env.close()