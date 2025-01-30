% Parameters
m = 0.313;         % Total mass (kg)
g = -9.81;     % Gravitational acceleration (m/s^2)
l = 0.09;      % Length (m)
I_P = m * (l * l); % 4.05e-3;       % Moment of inertia about pivot (kg·m^2)
I_F = 176.8e-6; % 9.28e-4;       % Moment of inertia of the reaction wheel (kg·m^2)

% Define the State-Cost Matrix (Q)
Q = diag([5, 2, 0.001, 0.5]); % cost of: theta, theta_dot, phi, phi_dot

% Define the Control-Cost Matrix (R)
R = 100000; % Weight on the control effort (u)

% Create A and B matrices
[A, B] = createAB(m, g, l, I_P, I_F);

if (checkControllable(A, B))
    % Compute LQR Gain Matrix
    K = createLQR(A, B, Q, R);
    % K = [-0.9, -0.14, 0, -0.001];
    disp('LQR State-Feedback Gain Matrix (K):');
    disp(K);

    x0 = [-0.1; 0; 0; 0]; % Initial conditions: small angle perturbation
    u_max = 0.2; % Maximum allowable control input (Nm)
    u_rate_max = 2.5; % Maximum rate of change (Nm/s). Will need to perform experiments to find this value
    dt = 0.01; % Seconds

    control_sys = ControlSimulator(A, B, K, u_max, u_rate_max, dt);
    % control_sys.checkControllability(x0, true);
    %control_sys.simulate(x0, true);

    x0 = control_sys.findThetaLimit(x0);
    control_sys.simulate(x0, true);

    % plotLyapunovROA(A, B, K, 'ThetaRange', [-(pi/8) (pi/8)], 'ThetaDotRange', [-0.5 0.5], 'N', 400, 'SafeLevel', 0.5);

end