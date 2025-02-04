import numpy as np


class LQReg(object):

  def __init__(self, K_1, K_2, K_3, K_4):
    self.setGains(K_1, K_2, K_3, K_4)

  def computeLQR(self, state):
    # State: [theta, theta_dot, phi, phi_dot]
    # Control: [tau]

    # apply gains to state
    tau = np.dot(-self.K_gains, state)
    return tau
  
  def setGains(self, K_1, K_2, K_3, K_4):
    self.K_gains = np.array([[K_1, K_2, K_3, K_4]])
