#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDateTime>

#include "dto/file/file_type.h"
#include "serialization/file/json.h"
#include "dto/file/metadata_response.h"
#include "dto/file/tree_response.h"

using namespace Serialization;

void testTreeSerialization();
void logTree(const QList<DTO::File::TreeNodeResponse>& list, int level = 0);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Common";

    qDebug() << "\n--- Testing MetadataResponse ---";
    DTO::File::MetadataResponse metaDto;
    metaDto.createdAt = QDateTime::currentDateTime();
    metaDto.type = DTO::File::FileType::File;
    metaDto.fileId = 42;
    metaDto.fileName = "book.pdf";
    metaDto.parentId = 10;
    metaDto.size = 1048576;

    QJsonObject metaJson = File::toJson(metaDto);
    auto parsedMeta = File::fromJsonMetadataResponse(metaJson);

    qDebug() << "[MetadataResponse] JSon object keys:" << metaJson.keys();
    qDebug() << "[MetadataResponse] parsed successfully:" << parsedMeta.has_value();
    if (parsedMeta.has_value()) {
        qDebug() << "   Extracted name:" << parsedMeta->fileName;
    }

    qDebug() << "\n--- End Testing MetadataResponse ---";

    testTreeSerialization();

    return 0;
}


void logTree(const QList<DTO::File::TreeNodeResponse>& list, int level)
{
    QString indent = QString(" ").repeated(level * 4);

    for (const auto& item : list)
    {
        QString icon = item.isDirectory ? "[Folder]" : "[File]";
        qDebug().noquote() << indent + icon << item.name << "(ID:" << item.fileId << ")";

        if (item.isDirectory && item.children.has_value())
        {
            logTree(item.children.value(), level + 1);
        }
    }
}
void testTreeSerialization()
{
    using namespace DTO::File;

    qDebug() << "\n=== Tree Test Started ===";

    // --- Build Tree Structure ---

    // Level 3: Deep nested files
    TreeNodeResponse photo1 { 301, "photo1.jpg", false, 2500000, std::nullopt };
    TreeNodeResponse photo2 { 302, "photo2.png", false, 1800000, std::nullopt };

    // Level 2: Subfolder and document files
    TreeNodeResponse backgroundsDir { 201, "Backgrounds", true, std::nullopt, QList<TreeNodeResponse>{ photo1, photo2 } };
    TreeNodeResponse doc1 { 202, "budget.xlsx", false, 15000, std::nullopt };
    TreeNodeResponse doc2 { 203, "report.docx", false, 25000, std::nullopt };

    // Level 1: Category folders
    TreeNodeResponse imagesDir { 101, "Images", true, std::nullopt, QList<TreeNodeResponse>{ backgroundsDir } };
    TreeNodeResponse docsDir { 102, "Documents", true, std::nullopt, QList<TreeNodeResponse>{ doc1, doc2 } };

    // Level 0: Root directory and files
    TreeNodeResponse mainFolder { 10, "Projects_Main", true, std::nullopt, QList<TreeNodeResponse>{ imagesDir, docsDir } };
    TreeNodeResponse emptyFolder { 10, "EmptyFolder", true, std::nullopt, QList<TreeNodeResponse>() };
    TreeNodeResponse rootFile1 { 11, "readme.txt", false, 1024, std::nullopt };
    TreeNodeResponse rootFile2 { 12, "setup.exe", false, 55000000, std::nullopt };

    QList<TreeNodeResponse> rootItems = { mainFolder, emptyFolder, rootFile1, rootFile2 };

    // --- Serialization (List -> QJsonArray) ---
    QJsonArray treeJsonArray = File::toJson(rootItems);
    QJsonDocument treeDoc(treeJsonArray);

    QFile file("test_tree_array_output.json");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(treeDoc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "[TreeNodeArray] JSON written to file.";
    }

    // --- Deserialization (QJsonArray -> List) ---
    auto parsedTreeOpt = File::fromJsonTreeNodeArray(treeJsonArray);
    qDebug() << "[TreeNodeArray] Success:" << parsedTreeOpt.has_value();
    qDebug() << "--- Tree ---";
    logTree(*parsedTreeOpt);

    qDebug() << "\n=== Tree Test Completed ===";
}
