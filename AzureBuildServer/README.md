#How to create a Jenkins Buildserver on Azure

This repo contains snippets of information on how to interact with MS Azure.

---

## Create a Virtual Machine from a cloud init file

This command uses the smallest VM size Azure offers and initializes the VM with
the file `jenkins-on-osdisk.yaml` meaning no sparate Data-Disk is created.  This
repo contains some cloud init files that create a minimal VM with Jenkins and
Docker.

    ```sh
	az vm create -g BuildServer --name JenkinsOnOsDisk  \
	--image Canonical:UbuntuServer:16.04-LTS:latest \
	--size Standard_A1   --admin-username thomas  \
	--ssh-key-value id_rsa.pub \
	--custom-data jenkins-on-osdisk.yaml
    ```

## Open and forward ports from the Internet to the VM

Per default a VM is not reachable from the Internet, to allow this we have to
open a TCP port.

	```sh
	az vm open-port -g BuildServer --name JenkinsOnOsDisk \
	--port 8080 --priority 1001
    ```
