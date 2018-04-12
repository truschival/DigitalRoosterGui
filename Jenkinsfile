
pipeline {
  
  environment { 
        BUILD_DIR = 'build'
        SOURCE_DIR = 'checkout'
    //    HTTP_PROXY= 'http://10.0.2.2:3128'
    //    HTTPS_PROXY= 'http://10.0.2.2:3128'
    }    
  
  agent {
        docker {
            image 'ruschi/devlinuxqtquick2:latest'
//            args  "--env HTTP_PROXY=${HTTP_PROXY} --env HTTPS_PROXY=${HTTPS_PROXY}"
        }
    } //agent
	
  stages {
    stage('Prepare') {
      steps {
        echo 'Preparing Environment'
        sh "mkdir -p ${BUILD_DIR}"
        sh "mkdir -p ${SOURCE_DIR}"

        dir("${SOURCE_DIR}"){
          git branch: 'develop',
                credentialsId: 'RuschisJenkins',
                url: 'https://git.rolf-dv.de/ruschi/DigitalRooster'
        }
      } // steps
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
            junit "test/gtestresults.xml"
        }
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
}// pipeline