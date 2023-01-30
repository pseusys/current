FROM python:3.10.9-slim-bullseye

ENV PYTHONUNBUFFERED 1
ENV PYTHONDONTWRITEBYTECODE 1
WORKDIR /3d-course-lab/

RUN apt-get update && apt-get install -y python3 python3-pip libglfw3-dev libassimp-dev x11-apps && rm -rf /var/lib/apt/lists/*
RUN pip3 install numpy Pillow PyOpenGL PyOpenGL-accelerate glfw cython AssimpCy

COPY src/* ./

ENTRYPOINT ["python3"]
CMD ["./main.py"]

