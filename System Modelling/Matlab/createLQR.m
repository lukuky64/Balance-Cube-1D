function [K, dominant_eigenvalue, dominant_eigenvector] = createLQR(A, B, Q, R)
    % Compute the LQR Gain Matrix
    K = lqr(A, B, Q, R);

    % Analyse Eigenvectors and Eigenvalues
    [T, D] = eig(A - B * K); % T: Eigenvectors, D: Eigenvalues
    eigen_vals = diag(D);    % Extract eigenvalues (complex)

    % Display the Eigenvalues of Closed-Loop System
    disp('Closed-Loop Eigenvalues of the LQR System:');
    disp(eigen_vals);

    % Find the Eigenvalue with the Largest Real Part (Most Dominant Mode)
    [~, max_idx] = max(real(eigen_vals)); % Index of the eigenvalue with the largest real part
    dominant_eigenvalue = eigen_vals(max_idx);

    % Extract the Corresponding Eigenvector
    dominant_eigenvector = T(:, max_idx);

    % Display the Dominant Eigenvalue and Corresponding Eigenvector
    disp('Dominant Eigenvalue (Largest Real Part):');
    disp(dominant_eigenvalue);
    disp('Corresponding Eigenvector:');
    disp(dominant_eigenvector);
end