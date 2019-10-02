function Component() {
    // default constructor
}

Component.prototype.createOperations = function() {
    component.createOperations();

    if (installer.value("os") === "win") {
       component.addOperation("CreateShortcut", "@TargetDir@/qlog.exe", "@StartMenuDir@/QLog.lnk");
    }
}