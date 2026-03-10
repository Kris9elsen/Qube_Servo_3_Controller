function dx = dynamics(~,x,J1,J2,m2,Lp,Lr,g,tau)

w1 = x(1);
w2 = x(2);
theta2 = x(3);

%% Accelerations

dw1 = tau / (J1 + m2*(Lp^2 + (Lr^2/4)*sin(theta2)));

dw2 = (m2*Lr^2*cos(theta2)*w1^2/8 - g*m2*sin(theta2)) / J2;

dtheta2 = w2;

dx = [dw1; dw2; dtheta2];

end