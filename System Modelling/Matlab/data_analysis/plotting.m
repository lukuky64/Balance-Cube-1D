% LOG_0.csv
% LOG_42.csv

data = readtable('LOG_42.csv');

% Access columns by their names
time_s = data.Time_s_;
theta_rad = data.Theta_rad_;
theta_dot_rad = data.theta_dot_rad_s_;
phi_rad = data.Phi_rad_;
phi_dot_rad = data.phi_dot_rad_s_;
setpoint_Nm = data.setpoint_A_;

polynomial_order = 2; % Order of the polynomial for the filter
window_size = 21;     % Window size (must be odd and greater than polynomial_order)
smoothed_theta_rad = sgolayfilt(theta_rad, polynomial_order, window_size);

% Plot the data
figure;
plot(time_s, smoothed_theta_rad, '-');
title('Data Plot');
xlabel('Time (s)');
grid on;