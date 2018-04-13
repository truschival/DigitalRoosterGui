pipeline {
	environment { 
        BUILD_DIR = 'build'
        SOURCE_DIR = 'checkout'
		 AN_ACCESS_KEY = credentials('RuschisJenkins') 
		// HTTP_PROXY= 'http://10.0.2.2:3128'
		// HTTPS_PROXY= 'http://10.0.2.2:3128'
    }    
    options {
        timeout(time: 10, unit: 'MINUTES')
        checkoutToSubdirectory('checkout')
		buildDiscarder(logRotator(numToKeepStr: '10', artifactNumToKeepStr: '10'))
    }
	
	agent {
        docker {
            image 'ruschi/devlinuxqtquick2:latest'
			//args  "--env HTTP_PROXY=${HTTP_PROXY} --env HTTPS_PROXY=${HTTPS_PROXY}"
        }
    }

	stages {
		stage('Prepare') {
			steps {
				echo 'Preparing Environment'
				sh "mkdir -p ${BUILD_DIR}"
				sh "mkdir -p ${SOURCE_DIR}"
			}
		}
		stage('Clone'){
			steps{
				checkout scm 
			}
		}
		stage('Configure'){
			steps{
				sh "echo U: $USER H: $HOME "
				sh "cmake -B${BUILD_DIR} -H${SOURCE_DIR} -DBUILD_TEST=On -DBUILD_GTEST_FROM_SRC=On"
			}
		}
		stage('Build') {
			steps {
				sh "cmake --build ${BUILD_DIR} --target all"
			}
		}
		stage('Test') {
			steps {
				// Testfiles are located in the build dir and referenced relative in source
				dir("${BUILD_DIR}"){  
					sh "cmake --build . --target test || true"
					sh "gcovr --xml -r . > coverage.xml"
					junit "test/gtestresults.xml"
				}
				step([$class: 'CoberturaPublisher',
					  autoUpdateHealth: false,
					  autoUpdateStability: false,
					  failUnhealthy: false,
					  failUnstable: false,
					  maxNumberOfBuilds: 0,
					  onlyStable: false,
					  sourceEncoding: 'ASCII',
					  zoomCoverageChart: false,
					  coberturaReportFile: "${BUILD_DIR}/coverage.xml"])
			}
		}
		stage('Package') {
			steps {
				sh "cmake --build ${BUILD_DIR} --target DOC"
				dir("${BUILD_DIR}"){ 
					sh "cpack -G TBZ2"
				}
				archiveArtifacts artifacts: "${BUILD_DIR}/*.tar.bz2", fingerprint: true
			}
		}
		stage('Cleanup') {
			steps {
				echo "Cleanup"
			}
		}
	} // stages
}//Jenkinsfile
