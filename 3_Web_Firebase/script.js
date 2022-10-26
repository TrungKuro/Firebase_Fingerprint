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