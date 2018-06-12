# C compiler
CC = gcc
CC_FLAGS = -fopenmp

SOURCES_CHECK=radiation_diana.cpp ../../include/log_helper.c
general:SOURCES_CHECK=radiation_diana.cpp ../../include/log_helper.c

general: simple_diana radiation_diana radiation_diana_timing

simple_diana: quicksort_gen.c generateInput.c
	$(CC) $(CC_FLAGS) quicksort_gen.c -o quick_gen
	$(CC) $(CC_FLAGS) generateInput.c -o genInput

quick_check: $(SOURCES_CHECK)
	$(CC) $(CC_FLAGS) $(SOURCES_CHECK) -DLOGS -o quick_check

quick_err_inj: $(SOURCES_CHECK)
	$(CC) $(CC_FLAGS) $(SOURCES_CHECK) -DLOGS -DERR_INJ -o quick_err_inj

quick_timing: $(SOURCES_CHECK)
	$(CC) $(CC_FLAGS) $(SOURCES_CHECK) -DTIMING -o quick_timing

clean:
	rm -f genInput quick_gen quick_check quick_err_inj quick_timing
