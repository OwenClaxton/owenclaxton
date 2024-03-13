% Initialisation
% ==================================
rng shuffle % reseed random number generator
nColumns = 100; % Number of Columns
nRows = 100; % Number of Rows
nCells = nColumns * nRows; % Number of Cells in the grid
nSteps = 300; % Number of Frames (steps) shown
pause_time = 0.00; % How many seconds to pause between frames

% Introducing variables for later use
num_of_herbs = 0;
num_of_carns = 0;

% The matrices will be 3D, however z is determined later

% ====================================================================
% Create a Main Matrix (M) that contains position and cell type data
% ====================================================================
M = zeros(nColumns, nRows, 3);

for n = 1:nColumns %For all columns
    for m = 1:nRows %For all rows
        v = round(10*rand(1)); %Create a random variable to decide cell type
        
        % Probability were determine based on reasonable approximations
        % of population sizes for predator and prey relationship
        if v <= 0.25
            M(n,m,1) = 1; %Set it as food
            
        elseif v > 0.25 && v <= 2
            M(n,m,2) = 1; %Set it as a herbivore
            num_of_herbs = num_of_herbs + 1; % Add 1 to the number of herbivore
            
        elseif v > 2 && v <= 4
            M(n,m,3) = 1; %Set it as a carnivore, else empty
            num_of_carns = num_of_carns + 1; % Add 1 to the number of carnivore
        end
        
    end
end

for q = 1:nSteps
    %Create Neighbour Matrix (N)
    N = M; % Duplicate the Main matrix
    
    %Define rearranged matrix section
    N_n = [nRows 1:nRows-1];     % indices of north neighbour
    N_e = [2:nColumns 1];       % indices of east neighbour
    N_s = [2:nRows 1];       % indices of south neighbour
    N_w = [nColumns 1:nColumns-1];     % indices of west neighbour
    
    %Creating rearranged matrices using definitions
    N_n_f = N(:, N_n, :);   % apply north neighbour rule to full matrix
    N_e_f = N(N_e, :, :);   % apply east neighbour rule to full matrix
    N_s_f = N(:, N_s, :);   % apply south neighbour rule to full matrix
    N_w_f = N(N_w, :, :);   % apply west neighbour rule to full matrix
    N_ne_f = N(N_e, N_n, :);   % apply north-east neighbour rule to full matrix
    N_nw_f = N(N_w, N_n, :);   % apply north-west neighbour rule to full matrix
    N_se_f = N(N_e, N_s, :);   % apply south-east neighbour rule to full matrix
    N_sw_f = N(N_w, N_s, :);   % apply south-west neighbour rule to full matrix
    
    % Create complete neighbour matrix
    N = N_n_f + N_e_f + N_s_f + N_w_f + N_ne_f + N_nw_f + N_se_f + N_sw_f;
    % Create non-cellular-specific neighbour matrix
    N_t = N(:,:,1) + N(:,:,2) + N(:,:,3);
    
    % ==================================
    % Apply Cellular Automata to M via N:
    % ==================================
    C = M; % Duplicate the Main matrix
    
    for n = 1:nColumns %For all columns
        for m = 1:nRows %For all rows
            %Herbivore Rules
            if M(n,m,1) == 0 && M(n,m,2) == 1 && M(n,m,3) == 0 %If the cell is a herbivore
                if N(n,m,2) <= 1 % If there is one or fewer herbivore neighbours
                    C(n,m,2) = 0; %Die from loneliness
                    
                elseif N_t(n,m) >= 4 % If there is 4 or more neighbours
                    C(n,m,2) = 0; %Die from overcrowding
                    
                else
                    %Do nothing
                end
                
                % ================================================
                % Decide whether or not herbivore survives carnivore
                % ================================================
                
                s = rand(); % introduce random variable
                
                % if the carnivore neighbour count > 4
                if N(n,m,3) >= 4
                    C(n,m,2) = 0; % kill it
                
                % if the carnivore neighbour count == 3
                elseif N(n,m,3) == 3
                    if s <= 0.90 % give 90% chance of death
                        C(n,m,2) = 0;
                    end
                    
                % if the carnivore neighbour count == 2
                elseif N(n,m,3) == 2
                    if s <= 0.75 % give 75% chance of death
                        C(n,m,2) = 0;
                    end
                    
                % if the carnivore neighbour count == 1
                elseif N(n,m,3) == 1
                    if s <= 0.60 % give 60% chance of death
                        C(n,m,2) = 0;
                    end
                end
                
            end
            
            % Carnivore Rules
            
            % If the cell is a carnivore
            if M(n,m,1) == 0 && M(n,m,2) == 0 && M(n,m,3) == 1
                %Create an excess-carnivore killer
                ratio_herb_to_carn = num_of_herbs/num_of_carns;
                death_ratio = rand(1);
                
                % This section increases the chance of death for fewer
                % herbivore numbers as this means less food for carnivore
                if ratio_herb_to_carn >= 1
                    %Continue
                elseif ratio_herb_to_carn >= 0.9 && death_ratio > 0.9
                    C(n,m,3) = 0;
                elseif ratio_herb_to_carn >= 0.8 && death_ratio > 0.8
                    C(n,m,3) = 0;
                elseif ratio_herb_to_carn >= 0.7 && death_ratio > 0.7
                    C(n,m,3) = 0;
                elseif ratio_herb_to_carn >= 0.6 && death_ratio > 0.6
                    C(n,m,3) = 0;
                elseif ratio_herb_to_carn >= 0.5 && death_ratio > 0.5
                    C(n,m,3) = 0;
                elseif ratio_herb_to_carn >= 0.4 && death_ratio > 0.4
                    C(n,m,3) = 0;
                elseif ratio_herb_to_carn >= 0.3 && death_ratio > 0.3
                    C(n,m,3) = 0;
                elseif ratio_herb_to_carn >= 0.2 && death_ratio > 0.2
                    C(n,m,3) = 0;
                elseif ratio_herb_to_carn >= 0.1 && death_ratio > 0.1
                    C(n,m,3) = 0;
                elseif num_of_herbs == 0 % kill carnivore as there is no food
                    C(n,m,3) = 0;
                end
                
                % ================================================
                % Decide whether or not carnivore survives naturally
                % ================================================
                
                % if the carnivore neighbour count <= 1
                if N(n,m,3) <= 1 % if there is 1 or less carnivore
                    C(n,m,3) = 0; %Die from loneliness
                    
                % if the neighbour count <= 7
                elseif N_t(n,m) >= 7 % if there is 7 or less carnivore
                    C(n,m,3) = 0; %Die from overcrowding
                    
                else
                    %Do nothing
                end
            end
            
            % Reproduction
            if (M(n,m,1) == 0) && (M(n,m,2) == 0) && (M(n,m,3) == 0) %If the cell is empty
                if (N(n,m,1) >= 1) %If there is a food source, make it easier to reproduce
                    priority = rand(1); % Introduce random variable
                    
                    if (N(n,m,2) >= 2) && (priority > 0.26) %If there are two herbivore neighbours
                        C(n,m,2) = 1; %Reproduce
                    elseif N(n,m,3) >= 3 %If there are three carnivore neighbours
                        C(n,m,3) = 1; %Reproduce
                    end
                    
                elseif N(n,m,1) == 0 %If there isn't a food source, make it harder to reproduce
                    
                    if (N(n,m,2) >= 3) && (priority > 0.26) %If there are three herbivore neighbours
                        C(n,m,2) = 1; %Reproduce
                    elseif N(n,m,3) >= 3 %If there are three carnivore neighbours
                        C(n,m,3) = 1; %Reproduce
                    end
                end
            end
            
        end
    end
    
    % Count numbers of herbivores and carnivores
    num_of_herbs = sum(sum(M(:,:,2)));
    num_of_carns = sum(sum(M(:,:,3)));
    
    % ==================================
    %  Apply (R)andom Walks to C via N,M:
    % ==================================
    
    R = C; % Duplicate the Cellular matrix
    array_order_cells = randperm(nCells); % Randomly arrange all cells
    
    
    % =================== Refer below to Live Script singular example =======================
    for chosen_cell = 1:(nCells)
        % Get the randomly chosen cell's x,y
        [x_chosen_cell, y_chosen_cell] = ind2sub([nColumns nRows], array_order_cells(chosen_cell));
        
        if R(x_chosen_cell,y_chosen_cell, 1) == 2 || ...
                R(x_chosen_cell,y_chosen_cell, 2) == 2 || ...
                R(x_chosen_cell,y_chosen_cell, 3) == 2
            %do nothing
            
        elseif R(x_chosen_cell,y_chosen_cell, 1) == 0 && ...
               R(x_chosen_cell,y_chosen_cell, 2) == 0 && ...
               R(x_chosen_cell,y_chosen_cell, 3) == 0
            %do nothing
            
        else
            neighbourhood = zeros(3,3);
            chosen_position_array = randperm(9);
            
            for option = 1:length(chosen_position_array)
                
                % This repeated check-for-a-cell-present is used to break the loop later
                if R(x_chosen_cell,y_chosen_cell, 1) == 1 || ...
                        R(x_chosen_cell,y_chosen_cell, 2) == 1 || ...
                        R(x_chosen_cell,y_chosen_cell, 3) == 1
                    [x_chosen_position_out, y_chosen_position_out] = ind2sub(size(neighbourhood), chosen_position_array(option));
                    x_chosen_position = x_chosen_position_out - 2 + x_chosen_cell;
                    y_chosen_position = y_chosen_position_out - 2 + y_chosen_cell;
                    z_chosen_position_foodsource = R(x_chosen_cell, y_chosen_cell, 1);
                    z_chosen_position_herbivore = R(x_chosen_cell, y_chosen_cell, 2);
                    z_chosen_position_carnivore = R(x_chosen_cell, y_chosen_cell, 3);
                    
                    % Check if cell is on border - if so, wrap around
                    if x_chosen_position == 0
                        x_chosen_position = nColumns;
                    end
                    if x_chosen_position == nColumns + 1
                        x_chosen_position = 1;
                    end
                    if y_chosen_position == 0
                        y_chosen_position = nRows;
                    end
                    if y_chosen_position == nRows + 1
                        y_chosen_position = 1;
                    end
                    
                    %See if space is empty and if so move there
                    if R(x_chosen_position,y_chosen_position, 1) == 0 && R(x_chosen_position,y_chosen_position, 2) == 0 && R(x_chosen_position,y_chosen_position, 3) == 0
                        R(x_chosen_position, y_chosen_position, 1) = 2*z_chosen_position_foodsource;
                        R(x_chosen_position, y_chosen_position, 2) = 2*z_chosen_position_herbivore;
                        R(x_chosen_position, y_chosen_position, 3) = 2*z_chosen_position_carnivore;
                        R(x_chosen_cell,y_chosen_cell, 1) = 0;
                        R(x_chosen_cell,y_chosen_cell, 2) = 0;
                        R(x_chosen_cell,y_chosen_cell, 3) = 0;
                    else
                        %Space must be full!
                    end
                else
                    %Cell must have moved, done!
                end
            end
        end
    end
    % =================== Refer above to Live Script singular example =======================
    
    % Remove 'Ignore Cell' Designation
    for n = 1:nColumns
        for m = 1:nRows
            for o = 1:3
                if R(n,m,o) == 2
                    R(n,m,o) = 1;
                end
            end
        end
    end
    
    % ================
    %  (V)isualisation
    % ================
    V = R; % Duplicating the Random walk matrix
    M = R; % Resetting the Main Matrix to the Random walk matrix
    
    % Assign every cell to their relevant colour
    for n = 1:nColumns
        for m = 1:nRows
            if V(n,m,1) == 1
                % Make it Yellow
                V(n,m,1) = 255; 
                V(n,m,2) = 255; 
                V(n,m,3) = 0;
                
            elseif V(n,m,2) == 1
                % Make it Green
                V(n,m,1) = 0; 
                V(n,m,2) = 255; 
                V(n,m,3) = 0;
                
            elseif V(n,m,3) == 1
                % Make it Red
                V(n,m,1) = 255; 
                V(n,m,2) = 0; 
                V(n,m,3) = 0;
                
            else
                % Make it White
                V(n,m,1) = 200; 
                V(n,m,2) = 200; 
                V(n,m,3) = 200;
            end
        end
    end
    
    % Draw frame
    imshow(V, 'InitialMagnification', 'Fit')
    drawnow
    pause(pause_time)
end