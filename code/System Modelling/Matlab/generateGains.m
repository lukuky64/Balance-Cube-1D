% Parameters
m = 0.2;         % Total mass (kg)
g = -9.81;     % Gravitational acceleration (m/s^2)
l = 0.09;      % Length (m)
I_P = 4.05e-3;       % Moment of inertia about pivot (kg·m^2)
I_F = 9.28e-4;       % Moment of inertia of the reaction wheel (kg·m^2)

% Define the State-Cost Matrix (Q)
Q = diag([5, 2, 0.01, 0.1]); % cost of: theta, theta_dot, phi, phi_dot

% Define the Control-Cost Matrix (R)
R = 5000; % Weight on the control effort (u)

% Create A and B matrices
[A, B] = createAB(m, g, l, I_P, I_F);

if (checkControllable(A, B))
    % Compute LQR Gain Matrix
    K = createLQR(A, B, Q, R);
    disp('LQR State-Feedback Gain Matrix (K):');
    disp(K);

    x0 = [0.3; 0.2; 0; 0]; % Initial conditions: small angle perturbation
    u_max = 5; % Maximum allowable control input (Nm)
    u_rate_max = 2; % Maximum rate of change (Nm/s). Will need to perform experiments to find this value

    % plotLyapunovROA(A, B, K, 'ThetaRange', [-(pi/4) (pi/4)], 'ThetaDotRange', [-1.5 1.5], 'N', 400, 'SafeLevel', 0.5);

    % Check controllability from the initial state
    checkControllableFromState(A, B, x0, u_max);

    % Simulate the system
    simulateSystem(A, B, K, x0, u_max, u_rate_max);

end