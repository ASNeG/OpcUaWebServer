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
      }
    }

    stage('test_linux') {
      steps {
        timeout(time: 5, unit: "MINUTES") {
          sh 'docker-compose run test_client bash -c "cd /code/ftest/ && nosetests --with-xunit"'
        }
      }
    }
  }

  post {
    always {
      sh 'docker-compose run test_client bash -c "find /code/ | grep __pycache__ | xargs rm -rf"'
      junit(testResults: 'ftest/nosetests.xml')

      sh 'docker-compose down --volumes --rmi local --remove-orphans'
      sh 'docker-compose run stack sh build.sh -t clean'
    }
  }
}
