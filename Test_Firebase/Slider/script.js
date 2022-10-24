// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
var firebaseConfig = {
    apiKey: "AIzaSyCGCNj5Y60_mOvVhGa59Z97Q_By_mz2CMY",
    authDomain: "nodemcu-c6e6d.firebaseapp.com",
    databaseURL: "https://nodemcu-c6e6d-default-rtdb.firebaseio.com",
    projectId: "nodemcu-c6e6d",
    storageBucket: "nodemcu-c6e6d.appspot.com",
    messagingSenderId: "238954531984",
    appId: "1:238954531984:web:86fc0e4646bf4898eedc15",
    measurementId: "G-TB1GL27H5M"
};

/* Initialize Firebase */
firebase.initializeApp(firebaseConfig);

/* Store all content element <input> had id... */
var sliderRed = document.getElementById("sliderRed");
var sliderGreen = document.getElementById("sliderGreen");
var sliderBlue = document.getElementById("sliderBlue");

/* Store all content element <div> had id... */
var SelectValueRed = document.getElementById("SelectValueRed");
var SelectValueGreen = document.getElementById("SelectValueGreen");
var SelectValueBlue = document.getElementById("SelectValueBlue");

/* Get value from element <input> had id "slider..." to display on element <div> had id "SelectValue..."
** document.getElementById("SelectValueRed").innerHTML = document.getElementById("sliderRed").value;
*/
SelectValueRed.innerHTML = sliderRed.value;
SelectValueGreen.innerHTML = sliderGreen.value;
SelectValueBlue.innerHTML = sliderBlue.value;

/* Get value from firebase to show it when first run (sync between html and firebase) */
var database = firebase.database();

/* FIREBASE SEND TO WEB TO GET VALUE
** firebase.database().ref().on("value", function (snap) {});
*/
database.ref().on("value", function (snap) {
    /*
    ** document.getElementById("sliderRed").value = snap.val().redValue;
    */
    sliderRed.value = snap.val().redValue;
    /*
    ** document.getElementById("SelectValueRed").value = snap.val().redValue;
    */
    SelectValueRed.innerHTML = snap.val().redValue;

    sliderGreen.value = snap.val().greenValue;
    SelectValueGreen.innerHTML = snap.val().greenValue;

    sliderBlue.value = snap.val().blueValue;
    SelectValueBlue.innerHTML = snap.val().blueValue;

    /* Change the background based on RGB value */
    document.getElementsByClassName('wrapper')[0].style.backgroundColor = `rgb(${sliderRed.value}, ${sliderGreen.value}, ${sliderBlue.value})`
});

/* ------------------------------------------------------------------------- */
/*          The oninput event occurs when an element gets user input         */
/* ------------------------------------------------------------------------- */

/*
** document.getElementById("sliderRed").oninput
*/
sliderRed.oninput = function () {
    /*
    ** document.getElementById("SelectValueRed").innerHTML = document.getElementById("sliderRed").value;
    */
    SelectValueRed.innerHTML = this.value;

    /* WEB SEND TO FIREBASE TO SET VALUE
    ** firebase.database().ref().child("redValue").set(document.getElementById("sliderRed").value);
    */
    var firebaseRef = firebase.database().ref().child("redValue");
    firebaseRef.set(sliderRed.value);
}

sliderGreen.oninput = function () {
    SelectValueGreen.innerHTML = this.value;

    var firebaseRef = firebase.database().ref().child("greenValue");
    firebaseRef.set(sliderGreen.value);
}

sliderBlue.oninput = function () {
    SelectValueBlue.innerHTML = this.value;

    var firebaseRef = firebase.database().ref().child("blueValue");
    firebaseRef.set(sliderBlue.value);
}
