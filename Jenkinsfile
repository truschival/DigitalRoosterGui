pipeline {
    environment { 
        BUILD_DIR = 'build'
        SOURCE_DIR = 'checkout'
        PWD = pwd()
    }    
    options {
        timeout(time: 10, unit: 'MINUTES')
        checkoutToSubdirectory('checkout')
        buildDiscarder(logRotator(numToKeepStr: '4', artifactNumToKeepStr: '8'))
    }
    
    agent {
        docker {
            image 'ruschi/devlinuxqtquick2:latest'
        }
    }

    stages {
        stage('Prepare') {
            steps {
                echo "Preparing Environment in $PWD"
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
                sh "echo U: $USER H: $HOME P: $PWD"
                sh "cmake -B${BUILD_DIR} -H${SOURCE_DIR} \
                    -DBUILD_TESTS=On \
                    -DBUILD_GTEST_FROM_SRC=On \
                    -DTEST_COVERAGE=On "
            }
        }
        stage('Build') {
            steps {
                sh "cmake --build ${BUILD_DIR} --target all"
            }
        }
        stage('Test') {
            steps {
                sh "cmake --build  ${BUILD_DIR} --target test || true"
                sh "gcovr -k -s -r ${SOURCE_DIR}       \
                --object-directory=${BUILD_DIR}        \
                --gcov-exclude='.*moc_.*'              \
                --gcov-exclude='.*automoc_dir.*'       \
                --gcov-exclude='.*/GTestExternal/.*'   \
                --gcov-exclude='/usr/.*'               \
                --xml-pretty -o coverage.xml "

                junit "${BUILD_DIR}/test/gtestresults.xml"
                
				step([$class: 'CoberturaPublisher',
                      autoUpdateHealth: false,
                      autoUpdateStability: false,
                      failUnhealthy: false,
                      failUnstable: false,
                      maxNumberOfBuilds: 0,
                      onlyStable: false,
                      coberturaReportFile: "coverage.xml"])
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
                deleteDir()
            }
        }
    } // stages
}//Jenkinsfile
