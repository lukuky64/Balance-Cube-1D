from stable_baselines3 import PPO
from InvertedPendulumEnv import InvertedPendulumEnv  # Import the class from the module

# Create an instance of your custom environment
env = InvertedPendulumEnv(render=True)

# Initialize the PPO model with the environment
model = PPO("MlpPolicy", env, verbose=1)

# Train the model
model.learn(total_timesteps=100000)

# Save the trained model
model.save("ppo_inverted_pendulum")

env.close()