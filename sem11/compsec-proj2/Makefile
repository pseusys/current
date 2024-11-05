build_image:
	docker build -t comp4634-proj2-image .
.PHONY: build_image

start_server:
	docker run -it --rm -p 3000:3000 --mount type=bind,source="$(pwd)"/code/router.js,target=/home/comp4634/proj2/router.js --mount type=bind,source="$(pwd)"/code/views,target=/home/comp4634/proj2/views --name bitbar-container comp4634-proj2-image
.PHONY: start_server


run:
	docker compose up --attach bitbar-server
.PHONY: run

submit:
	zip -r submission.zip attacks
.PHONY: submit
