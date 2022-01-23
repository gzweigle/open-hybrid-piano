% Gregary C. Zweigle, 2022
%
% This is temporary code to pull data from the two-file
% ping-pong buffer that has hammer sample data from PPS.
% After pulling the data, create a simple plot that moves
% approximately in real-time as data is received.
%
% Functionality is Matlab/Octave instead of with the
% Python code because it an experiment.
%
% This entire approach will be replaced by a web application
% in the future.

clear all;

rows_per_file = [4000, 8000];  % Must match Python value.
sample_rate = [500, 1000];
len_fifo = [16000, 32000];

f0 = samples_fifo(0, len_fifo(1), rows_per_file(1));
f1 = samples_fifo(1, len_fifo(2), rows_per_file(2));

while(1),

    f0 = f0.update_fifo();
    f1 = f1.update_fifo();

    t0 = [0:length(f0.fifo)+rows_per_file(1)-1]/sample_rate(1);
    t1 = [0:length(f1.fifo)+rows_per_file(2)-1]/sample_rate(2);

    % ADC values are 24-bit however only positive values are input to the ADC.
    % Therefore, divide by 2^23 to make full-scale 1.0.
    figure(1);
    subplot(2,1,1);
    plot(t0, f0.display_fifo / (2^23));
    axis([0 t0(end) 0 1])
    grid;

    subplot(2,1,2);
    plot(t1, f1.display_fifo / (2^23));
    axis([0 t1(end) 0 1])
    grid;

    pause(0.5);

end;