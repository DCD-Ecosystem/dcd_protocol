Docker image build and general usage sequence.
From the directory dcd_protokol/docker:


1. Initialize the docker image build:
docker build -f Dockerfile_dcd_protocol -t dcd_prot_img .

2. Bring up 5 nodes inside separate docker containers:
docker-compose up -d

3. Log in the genesis node and initialize the node setup, setting up producers and users:
docker exec -it node0 bash
/node/config/inited/init.sh
/node/config/inited/setproducers.sh

4. Inside logs of node0.
You can track the logs inside the node0 log workblockchain and worklog directories

5. Shutting the nerwork down
docker-compose down

In case you restart the data will be preserved and the blocks will be replayed
To start from scratch run delete_notedes_data.sh script

