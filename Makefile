CC ?= cc
CFLAGS = -Wall -Wextra -O3
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	OMP_FLAGS = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp
	LAPACK_FLAGS = -framework Accelerate
else ifeq ($(UNAME_S),Linux)
	OMP_FLAGS = -fopenmp
	LAPACK_FLAGS = -llapack
endif

ifneq ($(filter $(CC),cc gcc gcc-15),)
	CFLAGS += -flto
else ifeq ($(CC),icx)
	CFLAGS += -ipo
	OMP_FLAGS = -qopenmp -qopenmp-simd
endif
LDFLAGS = $(OMP_FLAGS) $(LAPACK_FLAGS)

SRC_FILES = src/*.c

DSYS ?= HH
ifeq ($(DSYS),HH)
	SIM_FOLDER = simulations/henon_heiles
else ifeq ($(DSYS),SP)
	SIM_FOLDER = simulations/spring_pendulum
endif


energy_error_check:
	$(CC) $(CFLAGS) $(SRC_FILES) $(SIM_FOLDER)/energy_error_check.c -o $(SIM_FOLDER)/energy_error_check.x $(LDFLAGS)

poincare_section:
	$(CC) $(CFLAGS) $(SRC_FILES) $(SIM_FOLDER)/poincare_section.c -o $(SIM_FOLDER)/poincare_section.x $(LDFLAGS)

grid_mle:
	$(CC) $(CFLAGS) $(SRC_FILES) $(SIM_FOLDER)/grid_mle.c -o $(SIM_FOLDER)/grid_mle.x $(LDFLAGS)

mle_vs_y:
	$(CC) $(CFLAGS) $(SRC_FILES) $(SIM_FOLDER)/mle_vs_y.c -o $(SIM_FOLDER)/mle_vs_y.x $(LDFLAGS)

grid_rte:
	$(CC) $(CFLAGS) $(SRC_FILES) $(SIM_FOLDER)/grid_rte.c -o $(SIM_FOLDER)/grid_rte.x $(LDFLAGS)

rte_vs_y:
	$(CC) $(CFLAGS) $(SRC_FILES) $(SIM_FOLDER)/rte_vs_y.c -o $(SIM_FOLDER)/rte_vs_y.x $(LDFLAGS)

clean:
	rm -rf $(SIM_FOLDER)/*.x
