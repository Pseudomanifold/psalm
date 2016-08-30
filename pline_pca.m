function Result = pline_pca(filename)

% Clear output file
file = fopen('Holes/pca.pline', 'w');
fclose(file);

i = 0; % Label counter might be wrong

file = fopen(filename, 'r');
while(~feof(file))
    line = fgetl(file);
    
    % Skip comment lines
    if(line(1) == '#')
        continue;
    end
    
    % Parse line
    parsed_line = sscanf(line, '%f');   % the first two column are actually 
                                        % integers, namely the label and
                                        % the number of vertices.
                   
    label_no = parsed_line(1);
    num_vertices = parsed_line(2);

    vertices_array  = parsed_line(3:3*num_vertices+2);
    vertices_matrix = zeros(num_vertices, 3);
    
    vertices_matrix(:,1) = vertices_array(1:3:end);
    vertices_matrix(:,2) = vertices_array(2:3:end);
    vertices_matrix(:,3) = vertices_array(3:3:end);
      
    [coeff, score, roots] = princomp(vertices_matrix);
    
    basis  = coeff(:,1:2); % first two columns form the basis of
                           % plane
    normal = coeff(:,3);   % normal of the plane
    
    % Compute error
    mean_vertices = mean(vertices_matrix, 1);
    error = abs((vertices_matrix-repmat(mean_vertices, num_vertices, 1))*normal);
   
    % Simple threshold criterion
    if(sum(error.^2) < 1e-05)
        out = fopen('Holes/pca.pline', 'a');
        fprintf(out, '%s\n', line);
        fclose(out);
    else
        continue;
    end

    % Plot points on plane
    projected_points = repmat(mean_vertices,num_vertices,1)+score(:,1:2)*coeff(:,1:2)';
    figure('visible', 'off');
    plot3(projected_points(:,1), projected_points(:,2), projected_points(:,3), 'bl');
    %plot3(vertices_matrix(:,1), vertices_matrix(:,2), vertices_matrix(:,3), 'bl');
    saveas(gcf, ['Holes/', num2str(label_no, '%03d'), '_', num2str(i, '%03d'), '.png'], 'png');
    
    out = fopen(['Holes/', num2str(label_no, '%03d'), '_', num2str(i, '%03d'), '.txt'], 'w');
    for j = 1:num_vertices
        fprintf(out, '%f %f %f %f %f\n', score(j,1), score(j,2),vertices_matrix(j,1), vertices_matrix(j,2), vertices_matrix(j,3));
    end
    fclose(out);
    
    i = i+1;
end
fclose(file);

end