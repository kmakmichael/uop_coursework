%% Fuzzy Clustering
% An implementation of the Fuzzy C-Means algorithm. Algorithm performs
% excellently and converges on a solution within a few iterations. Even
% when C is changed (anything above 3 will cause errors due to the plot
% coloring), algorithm places cluster centers very accurately.

% Parameters
load('clusterdata.mat'); 
C = 2; % number of clusters
m = 2; % fuzzifier;
uexp = 1/(m-1);
e = abs(max(max(clusterdemo))-min(min(clusterdemo)))/1000; % convergence threshold

% vectors
v = rand([C width(clusterdemo)]);
vo = zeros(C, width(clusterdemo));
u = zeros(height(clusterdemo), C);
dsq = ones(C, 1);

% functions
sqdist = @(x,y)(dot((x-y)', x-y));

% Main Loop
t = 0;
while sum(dsq) >= e || t > 100
    vo = v;
    for i = 1:height(clusterdemo)
        for k = 1:C
            dsq(k) = sqdist(clusterdemo(i,:), v(k,:));
            if dsq(k) == 0
                dsq(k) = 1;
                dsq(setdiff(1:C,k)) = 0;
                break;
            end
        end
        for k = 1:C
            u(i,k) = 1/sum((dsq(k)./dsq).^uexp);
        end
    end
    u = u.^m;
    for i=1:C
        v(i,:) = sum(u(:,i).*clusterdemo)./sum(u(:,i));
    end
    for k = 1:C
        dsq(k) = sqdist(vo(k,:), v(k,:));
    end
    t = t+1;
end

% Plot
figure();
hold on;
[~,mem] = max(u,[],2);
axis([0 1 0 1 0 1]);
grid on;
colors = [0.95 0.1 0.1; 0.1 0.7 0.1; 0.1 0.1 0.85;];
for k = 1:C
    scatter3( ...
        clusterdemo(mem==k,1), ...
        clusterdemo(mem==k,2), ...
        clusterdemo(mem==k,3), ...
        22, '.', 'MarkerEdgeColor', colors(k,:));
    scatter3(v(k,1), v(k,2), v(k,3), 24, 'o', 'LineWidth', 0.75, ...
        'MarkerFaceColor', colors(k,:), 'MarkerEdgeColor', [0 0 0]); 
end
view(40,35);
legend('Data', 'Centers');


