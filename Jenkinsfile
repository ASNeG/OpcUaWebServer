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
          sh 'docker-compose run webserver bash /tmp/ftest/WebGateway/test.sh'
        }
      }
    }
  }

  post {
    always {
      sh 'docker-compose down --volumes --rmi local'
    }
  }
}
