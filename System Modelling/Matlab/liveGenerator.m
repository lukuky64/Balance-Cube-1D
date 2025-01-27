% Const Parameters
m = 0.31;         % Total mass (kg)
g = -9.81;     % Gravitational acceleration (m/s^2)
l = 0.09;      % Length (m)
I_P = m * (l * l); % 4.05e-3;       % Moment of inertia about pivot (kg·m^2)
I_F = 9.28e-4;       % Moment of inertia of the reaction wheel (kg·m^2)

Q = diag([5, 2, 0.001, 0.1]); % cost of: theta, theta_dot, phi, phi_dot
R = 5000; % Weight on the control effort (u)

% Create A and B matrices
[A, B] = createAB(m, g, l, I_P, I_F);

if (checkControllable(A, B))
    % Compute LQR Gain Matrix
    K = createLQR(A, B, Q, R);
    disp('LQR State-Feedback Gain Matrix (K):');
    disp(K);

    x0 = [-0.3; 0; 0; 0]; % Initial conditions: small angle perturbation
    u_max = 0.2; % Maximum allowable control input (Nm)
    u_rate_max = 2; % Maximum rate of change (Nm/s). Will need to perform experiments to find this value
    dt = 0.01; % Seconds

    control_sys = ControlSimulator(A, B, K, u_max, u_rate_max, dt);
    % control_sys.checkControllability(x0, true);
    %control_sys.simulate(x0, true);

    x0 = control_sys.findThetaLimit(x0);
    control_sys.simulate(x0, true);

    % plotLyapunovROA(A, B, K, 'ThetaRange', [-(pi/8) (pi/8)], 'ThetaDotRange', [-0.5 0.5], 'N', 400, 'SafeLevel', 0.5);

end