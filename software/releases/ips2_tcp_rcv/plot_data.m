% Copyright (C) 2025 Greg C. Zweigle
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program. If not, see <https://www.gnu.org/licenses/>.
%
% Location of documentation, code, and design:
% https://github.com/gzweigle/open-hybrid-piano
% https://github.com/stem-piano
%
% plot_data.m
%
% Run after collecting data with tcp_ring_buffer.py
%
% directory_number = 0, 1, 2, or 3.
% scaling = 1 if want to view raw ADC counts.
% scaling = 32768 if want in range +/- 1.0 normalized.

function plot_data(directory_number, scaling)
num_notes = 96;
sample_rate = 1/300e-6;

directory_name = sprintf("data%d", directory_number);

test_data = load(sprintf("%s/data_0.txt", directory_name));
L = length(test_data);
t = [0:L-1]/sample_rate;

data = zeros(L,num_notes);
for ind = 0:(num_notes - 1)
    file_name = sprintf("%s/data_%d.txt", directory_name, ind);
    data(:,ind+1) = load(file_name);
end
data = data / scaling;

figure(1)
for half_octave = 0:3,
    subplot(2, 2, half_octave + 1);
    plot(t, data(:, 6*half_octave + 1 : 6*half_octave + 6));
    grid;
    if     half_octave == 0, title("A0 - Ef1");
    elseif half_octave == 1, title("E1 - Af1");
    elseif half_octave == 2, title("A1 - Ef2");
    else                     title("E2 - Af2");
    end;
end;
figure(2)
for half_octave = 4:7,
    subplot(2, 2, half_octave-4 + 1);
    plot(t, data(:, 6*half_octave + 1 : 6*half_octave + 6));
    grid;
    if     half_octave == 4, title("A2 - Ef3");
    elseif half_octave == 5, title("E3 - Af3");
    elseif half_octave == 6, title("A3 - Ef4");
    else                     title("E4 - Af4");
    end;
end;
figure(3)
for half_octave = 8:11,
    subplot(2, 2, half_octave-8 + 1);
    plot(t, data(:, 6*half_octave + 1 : 6*half_octave + 6));
    grid;
    if     half_octave == 8,  title("A4 - Ef5");
    elseif half_octave == 9,  title("E5 - Af5");
    elseif half_octave == 10, title("A5 - Ef6");
    else                      title("E6 - Af6");
    end;
end;
figure(4)
for half_octave = 12:15,
    subplot(2, 2, half_octave-12 + 1);
    plot(t, data(:, 6*half_octave + 1 : 6*half_octave + 6));
    grid;
    if     half_octave == 12, title("A6 - Ef7");
    elseif half_octave == 13, title("E7 - Af7");
    elseif half_octave == 14, title("A7 - C8, plus 2 pedal pins");
    else                      title("top 6 pedal pins");
    end;
end;