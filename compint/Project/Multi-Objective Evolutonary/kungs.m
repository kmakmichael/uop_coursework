function [execTime] = kungs(params)
    %% Params
    MaxIt = params.MaxIt;
    pCrossover = parems.pCrossover;
    nCrossover = 2*round(pCrossover*nPop/2);
    pMutation = 0.4;
    nMutation = round(pMutation*nPop);
    mu = 0.02;
    sigma = 0.1*(VarMax-VarMin);

    nPop = params.nPop;
    %% Initialization

    empty_individual.Position = [];
    empty_individual.Cost = [];
    empty_individual.Rank = [];
    empty_individual.DominationSet = [];
    empty_individual.DominatedCount = [];
    empty_individual.CrowdingDistance = [];

    pop = repmat(empty_individual, nPop, 1);

    for i = 1:nPop
        pop(i).Position = unifrnd(VarMin, VarMax, VarSize);
        pop(i).Cost = CostFunction(pop(i).Position);
    end
    
    % Non-Dominated Sorting
    [pop, F] = NonDominatedSorting(pop);

    % Calculate Crowding Distance
    pop = CalcCrowdingDistance(pop, F);

    % Sort Population
    [pop, F] = SortPopulation(pop);

    %% Main Loop

    for it = 1:MaxIt

        % Crossover
        popc = repmat(empty_individual, nCrossover/2, 2);
        for k = 1:nCrossover/2

            i1 = randi([1 nPop]);
            p1 = pop(i1);

            i2 = randi([1 nPop]);
            p2 = pop(i2);

            [popc(k, 1).Position, popc(k, 2).Position] = Crossover(p1.Position, p2.Position);

            popc(k, 1).Cost = CostFunction(popc(k, 1).Position);
            popc(k, 2).Cost = CostFunction(popc(k, 2).Position);

        end
        popc = popc(:);

        % Mutation
        popm = repmat(empty_individual, nMutation, 1);
        for k = 1:nMutation

            i = randi([1 nPop]);
            p = pop(i);

            popm(k).Position = Mutate(p.Position, mu, sigma);

            popm(k).Cost = CostFunction(popm(k).Position);

        end

        % Merge
        pop = [pop
             popc
             popm]; %#ok
        if kungs  
            % sort by 1st objective
            carr = [pop.Cost];
            [~, idx] = sort(carr(1,:), 2, 'ascend');
            pop = pop(idx);

            % deterine fronts
            idx = 1:height(pop);
            fr = 1;
            while size(idx) > 0
                [~,F{fr}] = Front(pop(idx), idx);
                for fi = F{fr}
                    pop(fi).Rank = fr;
                end

                idx = setdiff(idx, F{fr});
                fr = fr + 1;
            end

            % truncate
            pop = CalcCrowdingDistance(pop, F);
            [pop, F] = SortPopulation(pop);
            pop = pop(1:nPop);

            % sort once more
            [pop, F] = SortPopulation(pop);

            % store F1
            F1 = pop(F{1});
        else
            % Non-Dominated Sorting
            [pop, F] = NonDominatedSorting(pop);

            % Calculate Crowding Distance
            pop = CalcCrowdingDistance(pop, F);

            % Sort Population
            pop = SortPopulation(pop);

            % Truncate
            pop = pop(1:nPop);

            % Non-Dominated Sorting
            [pop, F] = NonDominatedSorting(pop);

            % Calculate Crowding Distance
            pop = CalcCrowdingDistance(pop, F);

            % Sort Population
            [pop, F] = SortPopulation(pop);

            % Store F1
            F1 = pop(F{1});
        end