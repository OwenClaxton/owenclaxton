close all;
clc;
clear;

%% Todo:
% - Add angle information (multiple vertices)

%% User guide
% To switch out datasets, change the variable data_set
%
% Interact with the figure using the mouse
%   1. Middle click, or shift click, starts the interactive drawing mode (IDM)
%   2. Right click quits IDM
%   3. Once in IDM, left click to start drawing a triangle from a vertex
%   4. Each triangle requires two clicks; once at a vertex (to start), and
%   then at the centre of the triangle (to finish). Whilst drawing, the
%   triangle should be visualised as a drawing aid.
%   5. Outside of IDM, you can drag triangle vertices to reshape the
%   triangle
%   6. Outside of IDM, you can right click on triangles and select delete 
%   polygon to delete triangles.
%   7. When you close the figure, you will be faced with a prompt that asks
%   to confirm the closure. By clicking yes, all triangles drawn will be
%   printed to file.csv in the directory of this script. If you click no,
%   nothing will happen.

%% Clean Data
data_set = 'E4-MS-E5GU-0_00000.0_00015.xyz';
global data;
data_layer = flip(xyz2grid(data_set));
data = repmat(data_layer, 1, 1, 3);

global shapes; % list of drawn shapes
shapes = [];

global prevClick; % list of click coordinates
prevClick = [];

global clickType; % last click type
clickType = 0;

global clickColour; % current colour for clicking
clickColour = 'red';

global fig;

%%
fig = figure('CloseRequestFcn', @closereqfcn);
    tiledlayout(1, 1, 'Padding', 'none', 'TileSpacing', 'compact');
        nexttile;
            imshow(doNormalise(data), 'Parent', fig.Children(end).Children(end));
            fig.WindowButtonDownFcn = {@mouse_click};
            fig.WindowButtonMotionFcn = {@mouse_move};
            
%% Print data
function csv_matrix = print_data
    global shapes data;
    csv_matrix = [];
    for i = 1 : length(shapes)
        if isvalid(shapes(i))
            coordinates = reshape(shapes(i).Position', 1, 6);
            angle = calc_tri_click_angles(coordinates);
            area = polyarea(shapes(i).Position(:,1), shapes(i).Position(:,2));
            areapercent = (area/(size(data, 1) * size(data, 2))) * 100;
            csv_matrix = [csv_matrix; coordinates, angle, area, areapercent];
        end
    end
    filename = make_file_name();
    writecell({'x1', 'y1', 'x2', 'y2', 'x3', 'y3', 'angle', 'area', 'areapercent'}, filename, 'WriteMode', 'overwrite')
    writematrix(csv_matrix, filename, 'WriteMode', 'append')
end
%% Make file name
function filename = make_file_name()
    tt = datetime(now, 'ConvertFrom', 'datenum');
    filename = ['file', num2str(tt.Year, '%04i'), ...
        num2str(tt.Month, '%02i'), num2str(tt.Day, '%02i'), ...
        num2str(tt.Hour, '%02i'), num2str(tt.Minute, '%02i'), ...
        num2str(floor(tt.Second), '%02i'), '.csv'];
end
%% Close Request
function closereqfcn(~, ~)
    % Close request function 
    % to display a question dialog box 
    selection = questdlg('Close This Figure? (This will print the results)', ...
        'Close Request Function', ...
        'Yes','No','Yes'); 
    switch selection   
        case 'Yes'
            print_data;
            delete(gcf)
        case 'No'
            return 
    end
end
%% Mouse click
function mouse_click(object, ~)
    global prevClick clickType clickColour fig shapes; % introduce global handles
    pt = get(fig.Children(end).Children(end), 'CurrentPoint');
    x = pt(1, 1);
    y = pt(1, 2);
    
    if any([x, y] > 512) || any([x, y] < 0)
        return
    end
    if strcmp(object.SelectionType, 'extend') % middle, shift+left, or left+right
        clickType = 1;
        clickColour = 'red';
        
    elseif strcmp(object.SelectionType, 'normal') % left click
        if clickType > 0
            clickType = clickType + 1;
        end
        if clickType == 1
            clickColour = 'red'; % first click will show a red circle
        
        elseif clickType == 4
            clickType = 2;
            
        elseif clickType == 3
            clickColour = 'blue'; %
            
            pt_1 = prevClick;
            pt_2 = rot_mat_point(pt_1, 120, [x, y]);
            pt_3 = rot_mat_point(pt_1, 240, [x, y]);
            new_shape = drawpolygon('Position', [pt_1; pt_2; pt_3], 'InteractionsAllowed', 'all', 'Parent', fig.Children(end).Children(end));
            % https://au.mathworks.com/help/images/ref/drawpolygon.html
            shapes = [shapes; new_shape];
        end
        
        if clickType == 2
            clickColour = 'green';
            
            prevClick = [x, y];
        end
        
    elseif strcmp(object.SelectionType, 'alt') % right click or ctrl + left
        clickType = 0;
        clickColour = 'red';
    else % double click
        %disp(object.SelectionType) %% 'open'
    end
    if clickType == 0
        x = -10;
        y = -10;
    end
    showShape('circle', [x, y, 3], 'Color', clickColour, 'Opacity', 0.1, 'Parent', fig.Children(end).Children(end));
end
%% Mouse move
function mouse_move(~, ~)
    global prevClick clickType clickColour fig;
    pt = get(fig.Children(end).Children(end), 'CurrentPoint');
    x = pt(1, 1);
    y = pt(1, 2);
    if any([x, y] > 512) || any([x, y] < 0)
        return
    end
    disp(['Mouse at coordinates: ', num2str(x), ', ', num2str(y), '.'])
    
    if clickType == 2
        
        pt_1 = prevClick;
        pt_2 = rot_mat_point(pt_1, 120, [x, y]);
        pt_3 = rot_mat_point(pt_1, 240, [x, y]);
        
        showShape('polygon', [pt_1, pt_2, pt_3], 'Color', 'red', 'Opacity', 0.4, 'Parent', fig.Children(end).Children(end));

    elseif clickType > 0
        
        showShape('circle', [x, y, 3], 'Color', clickColour, 'Opacity', 0, 'Parent', fig.Children(end).Children(end));
        
    end
    
end
%%
function angles = calc_tri_click_angles(clicks)
    angles = zeros(size(clicks, 1), 1);
    for i = 1 : size(clicks, 1)
        w = clicks(i, 1) - clicks(i, 3);
        h = clicks(i, 2) - clicks(i, 4);
        angles(i) = mod(rad2deg(atan2(h, w)) + 360, 120);
    end
end
%% OLD. Error in formula, replaced by polyarea() inbuilt function
function areas = calc_tri_click_areas(clicks)
    areas = zeros(size(clicks, 1), 1);
    for i = 1 : size(clicks, 1)
        areas(i) = sqrt(27/16) * abs(((clicks(i, 1) - clicks(i, 3)) ^ 2) + ((clicks(i, 2) - clicks(i, 4)) ^ 2));
    end
end
%%
function varargout = xyz2grid(varargin)

narginchk(1,inf) 
if isnumeric(varargin{1})
   x = varargin{1}; 
   y = varargin{2}; 
   z = varargin{3}; 
else
   [x,y,z] = xyzread(varargin{:}); 
end

assert(isequal(size(x),size(y),size(z))==1,'Dimensions of x,y, and z must match.') 
assert(isvector(x)==1,'Inputs x,y, and z must be vectors.') 

%% Grid xyz: 

% Get unique values of x and y: 
[xs,~,xi] = unique(x(:),'sorted'); 
[ys,~,yi] = unique(y(:),'sorted'); 

% Before we go any further, we better make sure we're not gridding scattered data. 
% This is not a perfect assessor, but it's at least some kind of check: 
if numel(xs)==numel(z)
   warning 'It does not seem like the xyz dataset is gridded. You may be attempting to grid scattered data, but I will try to put it into a 2D matrix anyway. Check the output spacing of X and Y.';
end

% Sum up all the Z values that in each x,y grid point: 
Z = accumarray([yi xi],z(:),[],[],NaN); 

% Flip Z to match X,Y and to be correctly oriented in ij image coordinates: 
Z = flipud(Z); 

%% Package up the outputs: 

switch nargout 
   case 1 
      varargout{1} = Z; 
      
   case 3

      % Create a meshgrid of x and y:
      [varargout{1},varargout{2}] = meshgrid(xs,flipud(ys)); 
      varargout{3} = Z; 
      
   otherwise
      error('Wrong number of outputs.') 
end
end
%%
function [output] = rot_mat_point(input, angle, point)
    Rmat = rot_mat(angle);
    
    output = zeros(size(input));
    
    for i = 1 : size(input, 1)
        output(i, :) = (Rmat * (input(i, :) - point)')' + point;
    end
end
%%
function [output] = rot_mat(angle)
    ang = deg2rad(angle);
    output = [cos(ang), -sin(ang); sin(ang), cos(ang)];
end
%%
function [x,y,z] = xyzread(filename,varargin) 
%% Error checks: 

narginchk(1,inf) 
nargoutchk(3,3)
assert(isnumeric(filename)==0,'Input error: filename must ba a string.') 
assert(exist(filename,'file')==2,['Cannot find file ',filename,'.'])

%% Open file: 

fid = fopen(filename); 
T = textscan(fid,'%f %f %f',varargin{:}); 
fclose(fid);

%% Get scattered data: 

x = T{1}; 
y = T{2}; 
z = T{3}; 

end
%%
function output = doNormalise(input)
    output = input - min(input(:));
    output = output ./ max(output(:));
end