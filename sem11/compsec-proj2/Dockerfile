FROM node:14-buster


# Create app directory
ADD code/package*.json /home/comp4634/proj2/
WORKDIR /home/comp4634/proj2/

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copied
# where available (npm@5+)

RUN npm install

# Install SQLite3
RUN apt-get update --fix-missing && apt-get install -y sqlite3

# Bundle app source
COPY code/ ./

EXPOSE 3000
CMD [ "npm", "start" ]
