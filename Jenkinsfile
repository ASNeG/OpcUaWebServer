pipeline {
  agent any
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
          steps {
            script {
              sh('basename $PWD > DIRNAME.txt')
              env.BUILDDIRNAME = 'C:\\build\\' + readFile('DIRNAME.txt').trim()
            }

            sh 'ssh 127.0.0.1 -l vagrant -p 2222 "cd $BUILDDIRNAME && C:\\build_vs.bat -t local -B Release -i C:\\ASNeG -vs \\"Visual Studio 15 2017 Win64\\" -j 2"'
          }
        }
      }
    }

    stage('test_linux') {
      steps {
        timeout(time: 5, unit: "MINUTES") {
          sh 'docker-compose run test_client bash -c "cd /code/ftest/ && nosetests --with-xunit"'
        }

        junit(testResults: 'ftest/nosetests.xml')
      }

    }
  }

  post {
    always {
      sh 'docker-compose down --volumes --rmi local --remove-orphans'
      deleteDir()
    }
  }
}
