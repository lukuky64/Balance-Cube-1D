function [K, dominant_eigenvector] = createLQR(A, B, Q, R)
    % Compute the LQR Gain Matrix
    K = lqr(A, B, Q, R);

    % Analyse Eigenvectors and Eigenvalues
    [T, D] = eig(A - B * K); % T: Eigenvectors, D: Eigenvalues
    eigen_vals = diag(real(D));  % Extract real parts of eigenvalues
    
    % Display the Real Parts of Closed-Loop Eigenvalues
    disp('Closed-Loop Eigenvalues (Real) of the LQR System:');
    disp(eigen_vals);
    
    % Find the Position of the Largest Eigenvalue (Most Dominant Mode)
    [~, max_idx] = max(abs(eigen_vals)); % Find the index of the largest eigenvalue

    % Display the Dominant Eigenvector (Corresponding to the Largest Eigenvalue)
    disp('Eigenvector Corresponding to the most stable Eigenvalue):');
    dominant_eigenvector = T(:, max_idx); % Select the eigenvector corresponding to the largest eigenvalue
    disp(dominant_eigenvector);
end