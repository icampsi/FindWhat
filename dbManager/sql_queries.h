#ifndef SQL_QUERIES_H
#define SQL_QUERIES_H

#include <QString>

namespace SqlQueries {
    /*
     * Template function to translate an enumType to int, but named as a function
     * to get the column index since that's its intended use
    */
    template <typename EnumType>
    inline int cIndex(EnumType column) { return static_cast<int>(column); }

    // ======================= MEMBERS =======================
    static const QString GetMembers = "SELECT * FROM Members";
    // Function to get a pointer to the query
    inline const QString* GetMembersQry() { return &GetMembers; }

    static const QString UpsertMembers =
        "INSERT INTO members (member_id, name, surname, dni, mail, phone, category) "
        "VALUES (:member_id, :name, :surname, :dni, :mail, :phone, :category) "
        "ON DUPLICATE KEY UPDATE "
        "name = VALUES(name), "
        "surname = VALUES(surname), "
        "dni = VALUES(dni), "
        "mail = VALUES(mail), "
        "phone = VALUES(phone), "
        "category = VALUES(category)";

    enum class MembersColumn {
        member_id,
        name,
        surname,
        dni,
        mail,
        phone,
        category
    };

    // ======================= RU_BILLS =======================
    static const QString GetRuBills =
        "SELECT "
        "ru_bills.pdf_hash, "
        "ru_bills.bill_num, "
        "ru_bills.exp_date, "
        "ru_bills.flat_id, "
        "CONCAT(flats.floor_num, ' - ', flats.door_num) AS flat, "
        "ru_bills.billing_periode_start, "
        "ru_bills.billing_periode_end, "
        "ru_bills.company_cif, "
        "utility_company.company_name, "
        "utility.utility_name, "
        "ru_bills.iva, "
        "ru_bills.bi, "
        "ru_bills.total "

        "FROM ru_bills "

        "LEFT JOIN utility ON "
        "ru_bills.utility_id = utility.utility_id "

        "LEFT JOIN flats ON "
        "ru_bills.flat_id = flats.flat_id "

        "LEFT JOIN utility_company ON "
        "ru_bills.company_cif = utility_company.company_cif";

    enum class GetRuBillsColumn {
        hash,
        bill_num,
        exp_date,
        flat_id,
        flatConcat_Floor_Door,
        billing_periode_start,
        billing_periode_end,
        company_cif,
        company_name,
        utility_name,
        iva,
        bi,
        total
    };

    static const QString EditRuBills =
        "SELECT "
        "ru_bills.pdf_hash, "
        "ru_bills.bill_num, "
        "ru_bills.exp_date, "
        "flats.flat_floor_num, "
        "flats.flat_door_num, "
        "ru_bills.billing_periode_start, "
        "ru_bills.billing_periode_end, "
        "ru_bills.company_cif, "
        "utility_company.company_name, "
        "utility.utility_name, "
        "ru_bills.iva, "
        "ru_bills.bi, "
        "ru_bills.total "

        "FROM ru_bills "

        "LEFT JOIN utility ON "
        "ru_bills.utility_id = utility.utility_id "

        "LEFT JOIN flats ON "
        "ru_bills.flat_id = flats.flat_id "

        "LEFT JOIN utility_company ON "
        "ru_bills.company_cif = utility_company.company_cif"
        "LIMIT 0";

    enum class EditRuBillsColumn {
        hash,
        bill_num,
        exp_date,
        flat_floor,
        flat_door,
        billing_periode_start,
        billing_periode_end,
        company_cif,
        company_name,
        utility_name,
        iva,
        bi,
        total
    };

    static const QString UpsertRuBills =
        "INSERT INTO ru_bills (pdf_hash, bill_num, exp_date, flat_id, billing_periode_start, "
        "billing_periode_end, company_cif, utility_id, iva, bi, total) "

        "VALUES (:pdf_hash, :bill_num, :exp_date, :flat_id, :billing_periode_start, "
        ":billing_periode_end, :company_cif, :utility_id, :iva, :bi, :total) "

        "ON DUPLICATE KEY UPDATE "
        "pdf_hash = VALUES(pdf_hash), "
        "exp_date = VALUES(exp_date), "
        "flat_id = VALUES(flat_id), "
        "billing_periode_start = VALUES(billing_periode_start), "
        "billing_periode_end = VALUES(billing_periode_end), "
        "company_cif = VALUES(company_cif), "
        "utility_id = VALUES(utility_id), "
        "iva = VALUES(iva), "
        "bi = VALUES(bi), "
        "total = VALUES(total)";

    enum class UpsertRuBillsColumn {
        hash,
        bill_num,
        exp_date,
        flat_id,
        billing_periode_start,
        billing_periode_end,
        company_cif,
        utility_id,
        iva,
        bi,
        total
    };

    // ======================= FLATS =======================
    static const QString GetFlats =
        "SELECT flat_id, floor_num, door_num, category, use_price, service_price, (use_price + service_price) AS Total FROM flats";

    static const QString UpsertFlats =
        "INSERT INTO flats (flat_id, floor_num, door_num, category, use_price, service_price) "

        "VALUES (:flat_id, :floor_num, :door_num, :category, :use_price, :service_price) "

        "ON DUPLICATE KEY UPDATE "
        "floor_num = VALUES(floor_num), "
        "door_num = VALUES(door_num), "
        "category = VALUES(category), "
        "use_price = VALUES(use_price), "
        "service_price = VALUES(service_price)";

    enum class FlatsColumn {
        flat_id,
        floor_num,
        door_num,
        category,
        use_price,
        service_price,
        total
    };

    // ======================= OCCUPANCY =======================
    static const QString GetOccupancy =
        "SELECT occupancy.occupancy_id, "
        "occupancy.flat_id, "
        "CONCAT(flats.floor_num, ' - ', flats.door_num) AS flat, "
        "occupancy.member_id, "
        "CONCAT(members.name, ' ', members.surname) AS full_name, "
        "occupancy.start_date, "
        "occupancy.end_date, "
        "occupancy.isMainHolder, "

        "CASE "
        "  WHEN occupancy.isMainHolder = 1 THEN 'Yes' "
        "  ELSE 'No' "
        "END AS MainHolder "

        "FROM occupancy "

        "LEFT JOIN flats "
        "ON occupancy.flat_id = flats.flat_id "

        "LEFT JOIN members "
        "ON occupancy.member_id = members.member_id";


    static const QString UpsertOccupancy =
        "INSERT INTO occupancy (occupancy_id, flat_id, member_id, start_date, end_date, isMainHolder) "

        "VALUES (:occupancy_id, :flat_id, :member_id, :start_date, :end_date, :isMainHolder) "

        "ON DUPLICATE KEY UPDATE "
        "flat_id = VALUES(flat_id), "
        "member_id = VALUES(member_id), "
        "start_date = VALUES(start_date), "
        "end_date = VALUES(end_date), "
        "isMainHolder = VALUES(isMainHolder)";

    enum class OccupancyColumn {
        occupancy_id,
        flat_id,
        flatConcat_Floor_Door,
        member_id,
        membersConcat_name_surname,
        start_date,
        end_date,
        isMainHolder,
        mainHolderTranslation
    };
}

#endif // SQL_QUERIES_H
