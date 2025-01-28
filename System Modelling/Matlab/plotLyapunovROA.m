function plotLyapunovROA(A, B, K, varargin)
% PLOTFYAPUNOVROA  Computes and plots an approximate Lyapunov-based Region
%                  of Attraction (ROA) for the closed-loop system x' = (A-BK)x.
%
% SYNTAX:
%   plotLyapunovROA(A, B, K);
%   plotLyapunovROA(A, B, K, 'Qlyap', Q_lyap, 'SafeLevel', 2.5, ...);
%
% DESCRIPTION:
%   1) Form A_cl = A - BK.
%   2) Solve the standard Lyapunov equation: A_cl * P + P * A_cl' + Q_lyap = 0.
%   3) On a 2D grid for (theta, theta_dot) with phi=0 and phi_dot=0, 
%      compute V(x) = x' * P * x and plot multiple contours.
%   4) Highlight 'SafeLevel' contour in red.
%
% INPUTS:
%   A, B : system matrices
%   K    : feedback gain
%
% OPTIONAL NAME-VALUE PAIRS:
%   'Qlyap':         Matrix Q for Lyapunov eqn (defaults to identity).
%   'ThetaRange':    [theta_min, theta_max] (defaults to [-0.5, 0.5])
%   'ThetaDotRange': [thetadot_min, thetadot_max] (defaults to [-2, 2])
%   'N':             Number of grid points in each dimension (defaults to 100)
%   'SafeLevel':     The c-level you consider "safe" (defaults to 1.0)
    %% Parse input arguments
    p = inputParser;
    p.addParameter('Qlyap', eye(size(A)), ...
        @(x) isnumeric(x) && all(size(x) == size(A)));
    p.addParameter('ThetaRange', [-0.2, 0.2], ...
        @(x) isnumeric(x) && numel(x) == 2);
    p.addParameter('ThetaDotRange', [-0.5, 0.,5], ...
        @(x) isnumeric(x) && numel(x) == 2);
    p.addParameter('N', 100, @(x) isscalar(x) && x > 0);
    p.addParameter('SafeLevel', 1.0, @(x) isscalar(x) && x > 0);
    p.parse(varargin{:});

    Q_lyap        = p.Results.Qlyap;
    thetaRange    = p.Results.ThetaRange;
    thetaDotRange = p.Results.ThetaDotRange;
    N             = p.Results.N;
    safeContour   = p.Results.SafeLevel;

    %% Form closed-loop system
    A_cl = A - B * K;

    %% Solve Lyapunov equation: A_cl * P + P * A_cl' + Q_lyap = 0
    % => A_cl' P + P A_cl = - Q_lyap
    P = lyap(A_cl, Q_lyap);

    % Ensure P is positive definite
    Peigs = eig(P);
    if any(Peigs <= 0)
        warning('[plotLyapunovROA] P might not be positive definite. Check stability & Q_lyap.');
    end

    %% Create 2D grid for (theta, theta_dot)
    thetaVals    = linspace(thetaRange(1), thetaRange(2), N);
    thetaDotVals = linspace(thetaDotRange(1), thetaDotRange(2), N);

    % Evaluate V(x) = x' P x at each grid point (phi=0, phi_dot=0)
    valMatrix = zeros(N, N);
    for i = 1:N
        for j = 1:N
            xTest = [thetaVals(i); thetaDotVals(j); 0; 0];
            valMatrix(j, i) = xTest' * P * xTest; 
        end
    end

    %% Plot
    figure('Name','Lyapunov ROA','Color','w');
    hold on; grid on;
    
    % 1) Plot multiple contours for reference
    contourLevels = [0.001, 0.01, 0.05, 0.12, 0.25, 0.5, 1, 2, 4, 8];  % you can expand/modify
    [C, h] = contour(thetaVals, thetaDotVals, valMatrix, ...
                     contourLevels, 'LineWidth', 1.5, 'LineColor','b');
    clabel(C, h, 'FontSize', 10, 'Color','k', 'LabelSpacing', 200, ...
           'Interpreter', 'none');
    
    % 2) Highlight the "safe" contour in red
    [Csafe, hSafe] = contour(thetaVals, thetaDotVals, valMatrix, ...
                             [safeContour safeContour], ...
                             'LineWidth', 2, 'LineColor', 'r');
    clabel(Csafe, hSafe, 'FontSize',10, 'Color','r', 'LabelSpacing',200, ...
           'Interpreter','none');
    
    % 3) Beautify
    xlabel('\theta (rad)');
    ylabel('\theta_{dot} (rad/s)');
    title('Lyapunov-Based Approx. of Region of Attraction');
    legend([hSafe], {sprintf('Safe Contour: V(x)=%.2f', safeContour)}, ...
        'Location','best');
    axis tight; 
end