stage('Checkout') {
    node {
        checkout scm
    }
}

stage('Pre-build') {
    node {
        echo "Node Name: ${env.NODE_NAME}"
        echo "Branch Name: ${env.BRANCH_NAME}"
        sh 'which g++'
    }
}

stage('Build') {
    node {
        echo 'Dummy build stage'
    }
}