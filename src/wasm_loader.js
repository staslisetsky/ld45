Module['onRuntimeInitialized'] = function(text) {
    console.log('preRun');
};

function gameInitialized() {
    var loading = document.getElementById('loading');
    console.log(loading);
    loading.innerHTML = "";
    console.log('game initialized!');
}