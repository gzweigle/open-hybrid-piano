# Gregary C. Zweigle, 2022
#
# Store streaming hammer samples in a two-file ping-pong.

class PianoDatabase:

    def __init__(self, board_number, sample_rate, rows_per_file):
        self.file_rows_index = 0
        self.file_number = 0

        self.board_number = board_number
        self.sample_rate = sample_rate
        self.rows_per_file = rows_per_file

        self.file_cache = 'data\cache' + str(self.board_number) + "_" + str(1) + '.txt'
        open(self.file_cache, 'w').close()  # Make sure it is empty
        self.file_cache = 'data\cache' + str(self.board_number) + "_" + str(0) + '.txt'
        open(self.file_cache, 'w').close()  # Make sure it is empty

    def write_samples(self, got_samples, samples, board_number, diff_time):

        # Hammer samples are only sent during strikes.
        # In between strikes, insert zeros to "keep time progressing".
        # This is an approximation, sufficient for preliminary testing.
        if got_samples == False:
            num_packets = round((diff_time/1e9) * self.sample_rate)
        elif self.board_number == board_number:
            num_packets = 1
        else:
            # Samples are for a different board so do nothing.
            num_packets = 0

        existing_file_row = self.file_rows_index + num_packets

        # If go past end of file, put the extras into the next file.
        if existing_file_row > self.rows_per_file:
            next_file_row = existing_file_row - self.rows_per_file
            existing_file_row = self.rows_per_file
            start_new_file = True
        else:
            next_file_row = 0
            start_new_file = False

        # Write data into the existing file that is open.
        empty_sample = [0] * 8
        with open(self.file_cache, 'a') as writer:
            for k in range(existing_file_row  - self.file_rows_index):
                if got_samples == True:
                    self.write_file(writer, samples)
                else:
                    self.write_file(writer, empty_sample)
    
        # Received more samples than existing file maximum size.
        # Therefore, open the other file and start writing to it
        # with the extra samples.
        if start_new_file == True:
            self.file_number = (self.file_number + 1)%2
            self.file_cache = ('data\cache' + str(self.board_number) + "_" +
            str(self.file_number) + '.txt')
            with open(self.file_cache, 'w') as writer:
                for k in range(next_file_row):
                    if got_samples == True:
                        self.write_file(writer, samples)
                    else:
                        self.write_file(writer, empty_sample)

        if start_new_file == True:
            self.file_rows_index = next_file_row
        else:
            self.file_rows_index = existing_file_row

    # The format of data in the file is simple. There are 8 columns
    # and each column is data for one piano hammer. For now, am not
    # including a timestamp or any additional metadata.
    # Each integer in a column will be in range [0, ..., 2^24-1].
    def write_file(self, writer, data):
        writer.write(
        "{:d} {:d} {:d} {:d} {:d} {:d} {:d} {:d}\n".format(
        data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]))
