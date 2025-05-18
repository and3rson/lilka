#include "lilcatalog.h"
#include <HTTPClient.h>
#include <lilka/config.h>

#include "utils/json.h"

    
LilCatalogApp::LilCatalogApp() : App("Лілка Талог") {    
    setStackSize(8192);    
    path_catalog_folder = "/lilcatalog";
    path_catalog_file = "/lilcatalog/catalog.json";        
    catalog_url = "https://pastebin.com/raw/YWYAwaEe";  
        
    catalog.clear();
    categoriesMenu.setTitle("Лілка Талог");    
}

void LilCatalogApp::run() {
    if (!lilka::fileutils.isSDAvailable()) {
        showAlert("SD карта не знайдена. Неможливо продовжити");
        lilka::serial_log("SD карта не знайдена");
        return;
    } else {
        lilka::serial_log("SD карта знайдена");
    }

    parseCatalog();
    while (1) {
        drawCatalog();
    }    
}

void LilCatalogApp::parseCatalog() {    
    catalog.clear();

    JsonDocument data(&spiRamAllocator);
    
    if (!SD.exists(path_catalog_folder)) {        
        lilka::serial_log("f_mkdir %d", path_catalog_folder);
        if (!SD.mkdir(path_catalog_folder.c_str())) {
            showAlert("Помилка створеня каталогу");
        }        
    }    

    if (!SD.exists(path_catalog_file)) {
        showAlert("Каталог не знайдено. Завантажте його з інтернету");        
    } else {        
        fs::File file = SD.open(path_catalog_file.c_str(), FILE_READ);
        String fileContent = "";
        
        while (file.available()) {
            fileContent += (char)file.read();
        }

        lilka::serial_log("Завантажено %d байт", fileContent.length());
        lilka::serial_log(fileContent.c_str());
        file.close();
        
        DeserializationError error = deserializeJson(data, fileContent);
        if (error) {
            showAlert("Помилка завантаження каталогу");
        } else {            
            lilka::serial_log("Категорії у каталозі: %d",data.size());
            for(int i = 0; i < data.size(); i++) {                                
                catalog_category category;
                category.name = data[i]["category"].as<String>();                

                for (int j = 0; j < data[i]["entries"].size(); j++) {
                    catalog_entry entry;
                    entry.name = data[i]["entries"][j]["name"].as<String>();
                    entry.description = data[i]["entries"][j]["description"].as<String>();
                    for(int k = 0; k < data[i]["entries"][j]["files"].size(); k++) {
                        catalog_entry_file file;
                        file.source = data[i]["entries"][j]["files"][k]["source"].as<String>();
                        file.target = data[i]["entries"][j]["files"][k]["target"].as<String>();
                        file.type = data[i]["entries"][j]["files"][k]["type"].as<String>();
                        entry.files.push_back(file);
                    }                    
                    category.entries.push_back(entry);
                    lilka::serial_log("Категорія: %s Контент: %s", category.name, entry.name);                
                }
                catalog.push_back(category);

                lilka::serial_log("Категорія: %s, кількість додатків: %d", category.name.c_str(), category.entries.size());                
            }
        } 
    }
}

void LilCatalogApp::fetchCatalog() {    
    lilka::Alert alert("lilcatalog", "Завантаження файлу...");
    alert.draw(canvas);
    queueDraw();

    WiFiClientSecure client;
    HTTPClient http;
    
    client.setInsecure();
    http.begin(client, catalog_url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        fs::File file = SD.open(path_catalog_file.c_str(), FILE_WRITE);
        if (!file) {
            showAlert("Помилка відкриття файлу");
            return;
        }        
        file.print(http.getString());
        file.close();

        delay(10);

        showAlert("Файл завантажено, та збережено");  
        parseCatalog();      
    } else {
        showAlert("Помилка. Помилка підключення" + http.errorToString(httpCode));        
    }
}

void LilCatalogApp::drawCatalog() {    
    categoriesMenu.clearItems();
    
    for(u_int i = 0; i < catalog.size(); i++) {        
        categoriesMenu.addItem(
            catalog[i].name.c_str(), 0, 0, "", 
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawCategory),
            reinterpret_cast<void*>(this)
        );            
    }
    categoriesMenu.addItem(
        "Завантажити", 0, 0, "", 
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchCatalog),
        reinterpret_cast<void*>(this)
    );        

    
    while (!categoriesMenu.isFinished()) {
        categoriesMenu.update();
        categoriesMenu.draw(canvas);
        queueDraw();
    }
}

void LilCatalogApp::drawCategory() {  
    u8_t categoryIndex = categoriesMenu.getCursor();
    catalog_category& category = catalog[categoryIndex];
    entriesMenu.setTitle(category.name.c_str());
    entriesMenu.clearItems();    
    for (u_int j = 0; j < category.entries.size(); j++) {
        entriesMenu.addItem(category.entries[j].name.c_str(), 0, 0, "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawEntry),
            reinterpret_cast<void*>(this));        
    }    
    entriesMenu.addItem("Назад", 0, 0, "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawCatalog),
            reinterpret_cast<void*>(this));
    
    while (!entriesMenu.isFinished()) {
        entriesMenu.update();
        entriesMenu.draw(canvas);
        queueDraw();
    }
}

void LilCatalogApp::drawEntry() {
    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry& entry = catalog[categoryIndex].entries[entryIndex];
    canvas->fillScreen(lilka::colors::Black);
    canvas->print(entry.description);
    lilka::display.drawCanvas(canvas);    

    lilka::Menu entryMenu("Вибір дії");  
    entryMenu.addItem("Встановити", 0, 0, "",
        reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::fetchEntry),
        reinterpret_cast<void*>(this));
    entryMenu.addItem("Назад", 0, 0, "",
            reinterpret_cast<lilka::PMenuItemCallback>(&LilCatalogApp::drawCategory),
            reinterpret_cast<void*>(this));

    while (!entryMenu.isFinished()) {
        entryMenu.update();
        entryMenu.draw(canvas);
        queueDraw();
    }
}

void LilCatalogApp::fetchEntry() {
    lilka::Alert alert("lilcatalog", "Завантаження файлу...");
    alert.draw(canvas);
    queueDraw();
    
    u8_t categoryIndex = categoriesMenu.getCursor();
    u8_t entryIndex = entriesMenu.getCursor();
    catalog_entry& entry = catalog[categoryIndex].entries[entryIndex];    

    WiFiClientSecure client;
    HTTPClient http;
    
    client.setInsecure();
    for(int i = 0; i < entry.files.size(); i++) {
        catalog_entry_file file = entry.files[i];        
        lilka::serial_log("Завантаження %s", file.source.c_str());
        lilka::serial_log("Збереження %s", file.target.c_str());

        createFoldersFromPath(file.target);   

        http.begin(client, file.source);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String destination = file.target;            
            fs::File file = SD.open(destination.c_str(), FILE_WRITE);
            
            if (!file) {
                showAlert("Помилка відкриття файлу");
                return;
            }
            file.print(http.getString());
            file.close();    
            
            delay(10);
        } else {
            showAlert("Помилка. Помилка підключення" + http.errorToString(httpCode));        
            return;
        }
    }    

    showAlert("Файл завантажено, та збережено");
}

void LilCatalogApp::showAlert(const String& message){
    lilka::Alert alert("lilcatalog", message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
    }
    lilka::serial_log(message.c_str());
}


void LilCatalogApp::createFoldersFromPath(const String &filePath) {
    if (!filePath.startsWith("/")) {
      lilka::serial_log("Invalid path. It should start with '/'");
      return;
    }
  
    int lastSlash = 0;
    String currentPath = "";
  
    
    // Loop through each segment between slashes
    while (true) {
      int nextSlash = filePath.indexOf('/', lastSlash + 1);
      if (nextSlash == -1) {
        // No more folders, stop before the file name
        int lastDot = filePath.lastIndexOf('.');
        if (lastDot != -1) {
          currentPath = filePath.substring(0, filePath.lastIndexOf('/'));
        }
        break;
      }
  
      currentPath = filePath.substring(0, nextSlash);
  
      if (!SD.exists(currentPath.c_str())) {
        if (SD.mkdir(currentPath.c_str())) {
            lilka::serial_log("Created folder: %s",currentPath);          
        } else {            
            showAlert("Поимлка створення директорії: " + currentPath);          
            return;
        }
      } else {
        lilka::serial_log("Folder already exists: %s", currentPath);        
      }
  
      lastSlash = nextSlash;
    }
}    
