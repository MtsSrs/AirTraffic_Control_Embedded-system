# BUILD
build-all: build-rgb build-lcd_buzzer build-infra build-thp

build-rgb:
	cd rgb_led && idf.py -p /dev/ttyUSB0 build

build-lcd_buzzer:
	cd lcd_buzzer && idf.py -p /dev/ttyUSB0 build

build-infra:
	cd infra && idf.py -p /dev/ttyUSB0 build

build-thp:
	cd thp && idf.py -p /dev/ttyUSB0 build

# RUN 
run-rgb:
	cd rgb_led && idf.py -p /dev/ttyUSB0 flash monitor

run-lcd_buzzer:
	cd lcd_buzzer && idf.py -p /dev/ttyUSB0 flash monitor

run-infra:
	cd infra && idf.py -p /dev/ttyUSB0 flash monitor

run-thp:
	cd thp && idf.py -p /dev/ttyUSB0 flash monitor

# CLEAN
clean-all: clean-rgb clean-lcd_buzzer

clean-rgb:
	cd rgb_led && idf.py fullclean

clean-lcd_buzzer:
	cd lcd_buzzer && idf.py fullclean

clean-infra:
	cd infra && idf.py fullclean

