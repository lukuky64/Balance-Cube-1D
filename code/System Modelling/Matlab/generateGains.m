% Parameters
m = 0.5;         % Total mass (kg)
g = -9.81;     % Gravitational acceleration (m/s^2)
l = 0.08;      % Length (m)
I_P = 0.5;       % Moment of inertia about pivot (kg·m^2)
I_F = 0.1;       % Moment of inertia of the reaction wheel (kg·m^2)

% Define the State-Cost Matrix (Q)
Q = [20  0   0   0;  % Cost of theta
     0  20  0   0;  % Cost of theta dot
     0   0   0.5   0;  % Cost of phi
     0   0   0   4]; % Cost of phi dot

% Define the Control-Cost Matrix (R)
R = 5; % Weight on the control effort (u)

% Create A and B matrices
[A, B] = createAB(m, g, l, I_P, I_F);

if (checkControllable(A, B))
    % Compute LQR Gain Matrix
    K = createLQR(A, B, Q, R);
    disp('LQR State-Feedback Gain Matrix (K):');
    disp(K);

    x0 = [0.25; 0.1; 0; 0]; % Initial conditions: small angle perturbation
    u_max = 0.5; % Maximum allowable control input (Nm)
    u_rate_max = 2; % Maximum rate of change (Nm/s). Will need to perform experiments to find this value

    % Check controllability from the initial state
    checkControllableFromState(A, B, x0, u_max);

    % Simulate the system
    simulateSystem(A, B, K, x0, u_max, u_rate_max);
end