e = ./main.py


prepare:
	xhost local:docker
.PHONY: prepare

load:
	docker build -t 3d-course-lab .
.PHONY: load

run-local: prepare
	docker build -t 3d-course-lab .
	docker run -it --network=host --rm -e DISPLAY=${DISPLAY} -v "/tmp/.X11-unix:/tmp/.X11-unix" -v "$XAUTH:/root/.Xauthority" 3d-course-lab $(e)
.PHONY: run-local

run: prepare
	docker build -t 3d-course-lab .
	docker run -it --network=host --rm -e DISPLAY=${DISPLAY} -v "/tmp/.X11-unix:/tmp/.X11-unix" -v "$XAUTH:/root/.Xauthority" 3d-course-lab $(e)
.PHONY: run
