pipeline {
  agent any
  triggers {
    pollSCM('* * * * *')
    cron('30 23 * * *') 
  }
  stages {
    stage('cppcheck') {
      steps {
        sh 'cppcheck --xml --xml-version=2 ./src 2> cppcheck.xml'
      }
    }
    stage('build') {
      parallel {
        stage('build_linux') {
          steps {
            sh 'docker-compose build --pull'
          }
        }

        stage('build_windows') {
          when {
               branch 'masterx'
          }
          steps {
            script {
              sh('basename $PWD > DIRNAME.txt')
              env.BUILDDIRNAME = 'C:\\build\\' + readFile('DIRNAME.txt').trim()
            }

            sh 'ssh 127.0.0.1 -l vagrant -p 2222 "cd $BUILDDIRNAME && C:\\build_vs.bat -t local -B Release -s C:\\ASNeG -i $BUILDDIRNAME\\ASNeG  -vs \\"Visual Studio 15 2017 Win64\\" -j 2"'
          }
        }
      }
    }

    stage('test_linux') {
      steps {
        timeout(time: 5, unit: "MINUTES") {
          sh 'docker-compose run test_client bash -c "cd /code/ftest/ && sh environment.sh && nosetests -s -v --with-xunit"'
        }

        junit(testResults: 'ftest/nosetests.xml')
      }

      post {
        always {
          sh 'docker-compose run test_client bash -c "find /code/ | grep __pycache__ | xargs rm -rf"'
        }
      }

    }
  }

  post {
    fixed {
      slackSend(color:'#BDFFC3', message:"Build Fixed - ${env.JOB_NAME} ${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)")
    }

    failure {
      slackSend(color:'#FF9FA1', message:"Build Failed - ${env.JOB_NAME} ${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)")
    }

    always {
      sh 'docker ps -a'
     
      sh 'docker cp $(docker ps -a -q -f "name=asneg-demo${BUILD_TAG}"):/var/log/OpcUaStack/ASNeG-Demo/OpcUaServer.log ASNeG-Demo.log || true'
      archiveArtifacts artifacts: 'ASNeG-Demo.log', fingerprint: true

      sh 'docker cp $(docker ps -a -q -f "name=webserver${BUILD_TAG}"):/var/log/OpcUaStack/OpcUaWebServer/OpcUaServer.log webserver.log || true' 
      archiveArtifacts artifacts: 'webserver.log', fingerprint: true
    }

    cleanup {
      sh 'docker-compose down --volumes --rmi local --remove-orphans'
      deleteDir()
    }
  }
}
