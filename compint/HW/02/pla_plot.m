function pla_plot(x, y, w)
    figure();
    n = size(x,1);
    hold on;
    for i = 1:n
        if y(i) == 1
            scatter(x(i,2),x(i,3),'r','+','LineWidth',2);
        else
            scatter(x(i,2),x(i,3),'g','O','LineWidth',2);
        end
    end
    axis([-1 1 -1 1])
    xlabel('x_1');
    ylabel('x_2');
    ax = gca;
    ax.XAxisLocation='origin';
    ax.YAxisLocation='origin';
    x1 = min(x(:,2)):0.1:max(x(:,2));
    y1 = -(w(2)*x1+w(1))/w(3);
    plot(x1, y1, 'Color',[.8 .8 .8], 'LineStyle', '--','LineWidth',2);
end