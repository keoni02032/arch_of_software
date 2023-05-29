USE cnfdb;
SET FOREIGN_KEY_CHECKS=0;
DROP TABLE ReportConference;
DROP TABLE Conference;
DROP TABLE Category;
DROP TABLE Report;
DROP TABLE User;

CREATE TABLE IF NOT EXISTS `User` (`id` INT NOT NULL AUTO_INCREMENT,
                        `first_name` VARCHAR(256) NOT NULL,
                        `last_name` VARCHAR(256) NOT NULL,
                        `login` VARCHAR(256) NOT NULL,
                        `password` VARCHAR(256) NOT NULL,
                        `email` VARCHAR(256) NULL,
                        `title` VARCHAR(1024) NULL,
                        PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));

CREATE TABLE IF NOT EXISTS `Report`(`id` int NOT NULL AUTO_INCREMENT,
                        `name_report`   text NOT NULL ,
                        `author_id`     int NOT NULL ,
                        `annotation`    text NOT NULL ,
                        `text_report`   text NOT NULL ,
                        `date_creation` text NOT NULL ,

                        PRIMARY KEY (`id`));
                        
CREATE TABLE IF NOT EXISTS `Category`(`id` int NOT NULL AUTO_INCREMENT,
                        `name_of_category` varchar(1024) NOT NULL ,
                        PRIMARY KEY (`id`));

CREATE TABLE IF NOT EXISTS `Conference`(`id` int NOT NULL AUTO_INCREMENT,
                        `name_conf`    varchar(1024) NOT NULL,
                        `organizer_id` int NOT NULL,
                        `category_id`  int NOT NULL,
                        `description`  text NOT NULL,
                        `date_of_conf` text NOT NULL,

                        PRIMARY KEY (`id`),
                        KEY `FK_2` (`category_id`),
                        CONSTRAINT `FK_1` FOREIGN KEY `FK_2` (`category_id`) REFERENCES `Category` (`id`));

CREATE TABLE IF NOT EXISTS `ReportConference`(`report_id` int NOT NULL ,
                        `conf_id`   int NOT NULL ,

                        PRIMARY KEY (`report_id`, `conf_id`),
                        KEY `FK_1` (`report_id`),
                        CONSTRAINT `FK_4` FOREIGN KEY `FK_1` (`report_id`) REFERENCES `Report` (`id`),
                        KEY `FK_3` (`conf_id`),
                        CONSTRAINT `FK_5` FOREIGN KEY `FK_3` (`conf_id`) REFERENCES `Conference` (`id`));

/*
SELECT * FROM Report;
SELECT * FROM ReportConference;
SELECT * FROM User;
SELECT * FROM Conference;
SELECT * FROM Category;

USE cnfdb;
SET FOREIGN_KEY_CHECKS=0;
truncate table ReportConference;
truncate table Conference;
truncate table Category;
truncate table Report;
truncate table User;
*/
use cnfdb;
INSERT INTO `User`
(main_id, login, password, first_name, last_name, email, title)
VALUES(1, 'Helen', 'helen', 'Helen', 'KarKarycheva', 'smeshar@yandex.ru', 'Mrs');

INSERT INTO `User`
(id, login, password, first_name, last_name, email, title)
VALUES(2, 'basta', 'moyaigra', 'Vasiliy', 'Vak', 'basta@yandex.ru', 'Mr');

INSERT INTO `User`
(id, login, password, first_name, last_name, email, title)
VALUES(3, 'vesna', 'music', 'Music', 'Vesna', 'music_vesna@yandex.ru', 'Mrs');

INSERT INTO Category
(name_of_category)
VALUES('International');
	  
INSERT INTO Category
(name_of_category)
VALUES('Russian');
	  
INSERT INTO Category
(name_of_category)
VALUES('Intrauniversity');

INSERT INTO Conference
(name_conf, organizer_id, category_id, description, date_of_conf)
VALUES('Gagarin readings', 1, 1, "MAI", "11-14 april 2023");
      
INSERT INTO Conference
(name_conf, organizer_id, category_id, description, date_of_conf)
VALUES('Kolachev readings', 1, 2, "SF MAI", "07 april 2023");

INSERT INTO Report
(name_report, author_id, annotation, text_report, date_creation)
VALUES('Believer', 1, '“Believer” thunders around a percussive backbeat and expressive vocals', 'First things first Ima say all the words inside my head Im fired up and tired of the way that things have been', '01.04.2023');

INSERT INTO Report
(name_report, author_id, annotation, text_report, date_creation)
VALUES('Meaning', 2, 'Kuzinatra', 'There is nothing to see there, nothing interesting!', '02.04.2023');
 
INSERT INTO Report
(name_report, author_id, annotation, text_report, date_creation)
VALUES('Nutracker!', 3, 'Violin', 'Winter', '31.03.2023');      

INSERT INTO ReportConference
(report_id, conf_id)
VALUES(1,2);

INSERT INTO ReportConference
(report_id, conf_id)
VALUES(1,1);

INSERT INTO ReportConference
(report_id, conf_id)
VALUES(2,2);