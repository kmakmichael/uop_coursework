function [pop, F] = UsefulSort(pop)
    [~, idx] = sort([pop.Cost], 2, 'descend');
    pop = pop(idx);
end