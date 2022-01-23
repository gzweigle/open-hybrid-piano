% Gregary C. Zweigle, 2022
%
% Pull data from 2-file buffer and shift through a FIFO.
%
% All code below is temporary.

classdef samples_fifo
  properties
    board_number;
    rows_per_file;
    fifo;
    display_fifo;
    len_fifo;
    file_number;
  end

  methods

    % Expect board_number >= 0.
    function obj = samples_fifo(board_number, len_fifo, rows_per_file)
      if (nargin > 0)
        obj.board_number = board_number;
        obj.rows_per_file = rows_per_file;
        obj.fifo = zeros(len_fifo,8);
        obj.len_fifo = len_fifo;
        obj.file_number = 0;
      end
    end

    function obj = update_fifo(obj)

      % Read in what is likely a partially completed file.
      % File name convention matches Python code.
      try
       data = load(['data\\cache' num2str(obj.board_number) '_'  ...
         num2str(obj.file_number) '.txt']);
      catch
        data = zeros(1,8);
      end;

      % Create a visual effect as follows:
      % While file is updating, display partial data to right side of plot.
      % Once file is fully updated, shift displayed data to the left.
      if length(data) == obj.rows_per_file,
        % If a complete file was read, switch to the other file for next time.
        obj.file_number = mod(obj.file_number + 1, 2); 
        % Shift the fifo left and put the new data at the end.
        obj.fifo = [obj.fifo(obj.rows_per_file+1:end,:); data];
        obj.display_fifo = [obj.fifo; zeros(obj.rows_per_file, 8)];
      else
        % Place the new data at end, do not move fifo.
        obj.display_fifo = [obj.fifo; ...
        data; zeros(obj.rows_per_file-length(data),8)];
      end;

    end;
  end;
end;