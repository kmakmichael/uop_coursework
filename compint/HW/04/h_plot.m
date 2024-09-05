function [] = h_plot(x, y, w)
    figure();
    n = size(x,1);
    hold on;
    for i = 1:n
        if y(i) == 1
            scatter(x(i,1),x(i,2),'r','+','LineWidth',2);
        else
            scatter(x(i,1),x(i,2),'g','O','LineWidth',2);
        end
    end
    axis([-1 1 -1 1])
    xlabel('x_1');
    ylabel('x_2');
    ax = gca;
    ax.XAxisLocation='origin';
    ax.YAxisLocation='origin';
    syms x1 x2
    eqn = w(1) + w(2)*x1 + w(3)*x2 + w(4)*x1^2 + w(5)*x1*x2 + w(6)*x2^2 + w(7)*x1^3 + w(8)*x1^2*x2 + w(9)*x1*x2^2 + w(10)*x2^3 == 0;
    fimplicit(eqn,[-1 1 -1 1],'LineWidth',2,'DisplayName','Hypothesis')
end