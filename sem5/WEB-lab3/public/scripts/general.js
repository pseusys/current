function get_params(param) {
    const params = new URLSearchParams(window.location.search);
    return params.get(param);
}

function collect_values_if_possible(...ids) {
    const id_array = [...ids];
    const necessary_fields = $(id_array.map(el => "#" + el).join(", "));
    let valid = true;
    necessary_fields.map(function () {
        $(this).toggleClass("is-invalid", this.value === "");
        valid &= (this.value !== "");
        return this;
    });
    if (valid) {
        const result = {};
        for (const field of necessary_fields) result[field.id] = field.value;
        return result;
    }
}
