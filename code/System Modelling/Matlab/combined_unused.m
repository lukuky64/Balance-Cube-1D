% Parameters
m = 1;         % Total mass (kg)
g = -9.81;     % Gravitational acceleration (m/s^2)
l = 0.15;      % Length (m)
I_P = 1;       % Moment of inertia about pivot (kg·m^2)
I_F = 1;       % Moment of inertia of the reaction wheel (kg·m^2)

% System Dynamics
f_1 = -(m * g * l) / (I_P + m * (l^2));
f_2 = (m * g * l) / (I_P + m * (l^2));
f_3 = -(1 / (I_P + m * (l^2)));
f_4 = 1 / I_F + 1 / (I_P + m * (l^2));

A = [0, 1, 0, 0;
     f_1, 0, 0, 0;
     0, 0, 0, 1;
     f_2, 0, 0, 0];

B = [0;
     f_3;
     0;
     f_4];

% Eigenvalues
eigs = eig(A);
disp('Eigenvalues of A:');
disp(eigs);

% Controllability Check
if rank(ctrb(A, B)) == size(A, 1)
    disp('System is controllable');
else
    disp('System is not controllable');
end

% Custom Pole Placement
eigs_custom = [-1.1; -1.2; -1.3; -1.4]; % The larger these are, the more agressive the controller
K_custom = place(A, B, eigs_custom);
disp('State-feedback gain matrix K:');
disp(K_custom);
eigs_custom_result = eig(A-B*K_custom);
disp('Eigenvalues of closed loop system:');
disp(eigs_custom_result);



% LQR Pole Placement for State Feedback Control

% Define the State-Cost Matrix (Q)
Q = [10  0   0   0;  % Cost of theta
     0  100  0   0;  % Cost of theta dot
     0   0   1   0;  % Cost of phi
     0   0   0   1]; % Cost of phi dot

% Define the Control-Cost Matrix (R)
R = 0.1; % Weight on the control effort (u)

% Compute the LQR Gain Matrix
K_lqr = lqr(A, B, Q, R);

% Display Results
disp('LQR State-Feedback Gain Matrix (K):');
disp(K_lqr);

% Closed-Loop System Eigenvalues
eigs_lqr = eig(A - B * K_lqr);
disp('Closed-Loop Eigenvalues of the LQR System:');
disp(eigs_lqr);

% Analyse Eigenvectors and Eigenvalues
[T, D] = eig(A - B * K_lqr); % T: Eigenvectors, D: Eigenvalues
eigen_vals = diag(real(D));  % Extract real parts of eigenvalues

% Display the Most Stable Mode (First Eigenvector)
disp('Most Stable Mode (First Eigenvector):');
disp(T(:, 1)); % First column of T corresponds to the most stable eigenvalue