function checkAccess(e) {
  event.preventDefault();

  var username = document.getElementById("uname").value;
  var password = document.getElementById("psw").value;

  if (username === "admin" && password === "123") {
    document.getElementById('access').style.display = "none";
  }
  else {
    alert("This account is not valid!\nPlease log in again.");
  }
}

function updateName(e) {
  event.preventDefault();

  var getName = document.getElementById("set-name").value;
  var getID = document.getElementById("set-id").value;

  alert(getName + " : " + getID);
}

function updateOpen(e) {
  event.preventDefault();

  var getOpen = document.getElementById("set-open").value;

  alert(getOpen);
}

function updateClose(e) {
  event.preventDefault();

  var getClose = document.getElementById("set-close").value;

  alert(getClose);
}