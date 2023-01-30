e = ./main.py

run:
	xhost local:docker
	docker build -t 3d-course-lab .
	docker run -it --network=host --rm -e DISPLAY=${DISPLAY} -v "/tmp/.X11-unix:/tmp/.X11-unix" -v "$XAUTH:/root/.Xauthority" 3d-course-lab $(e)

