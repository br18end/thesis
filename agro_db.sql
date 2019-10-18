CREATE DATABASE IF NOT EXISTS agroiotec;
USE agroiotec;
CREATE TABLE IF NOT EXISTS empresas(
idEmpresa INT PRIMARY KEY AUTO_INCREMENT NOT NULL, 
nombreEmpresa VARCHAR(40), 
razonSocial VARCHAR(40), 
representanteLegal VARCHAR(40), 
RFC VARCHAR(15), 
direccionFiscal VARCHAR(40), 
telefono INT, 
correoElectronico VARCHAR(20)
);
CREATE TABLE IF NOT EXISTS ranchos(
idRancho INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
nombreRancho VARCHAR(40),
direccionRancho VARCHAR(40),
descripcion VARCHAR(40),
idEmpresa INT,
FOREIGN KEY(idEmpresa) REFERENCES empresas(idEmpresa)
);
CREATE TABLE IF NOT EXISTS rangos(
idRango INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
maxGravitacional INT,
minGravitacional INT,
maxCapilar INT,
minCapilar INT,
maxHigroscopica INT,
minHigroscopica INT
);
CREATE TABLE IF NOT EXISTS celdas(
idCelda INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
idRancho INT,
FOREIGN KEY(idRancho) REFERENCES ranchos(idRancho),
idRango INT,
FOREIGN KEY(idRango) REFERENCES rangos(idRango)
);
CREATE TABLE IF NOT EXISTS tiposSensores(
idTipoSensor INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
tipoSensor VARCHAR(40),
descripcion VARCHAR(40)
);
CREATE TABLE IF NOT EXISTS sensores(
idSensor INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
idTipoSensor INT,
FOREIGN KEY(idTipoSensor) REFERENCES tiposSensores(idTipoSensor)
);
CREATE TABLE IF NOT EXISTS mediciones(
idMedicion INT PRIMARY KEY AUTO_INCREMENT NOT NULL,
fecha DATE,
hora TIME,
medicion FLOAT,
idSensor INT,
FOREIGN KEY(idSensor) REFERENCES sensores(idSensor),
idCelda INT,
FOREIGN KEY(idCelda) REFERENCES celdas(idCelda)
);
