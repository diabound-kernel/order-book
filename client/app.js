const ws = new WebSocket('ws://localhost:9002');
const asksContainer = document.getElementById("asks");
const bidsContainer = document.getElementById("bids");
const tradeHistoryList = document.getElementById("trade-history-list");
const orderForm = document.getElementById("order-form");

const ctx = document.getElementById("depth-chart").getContext("2d");
let depthChart;

// Render order rows
function renderOrderRow(side, price, quantity, total) {
    const row = document.createElement("div");
    row.classList.add("order-row", side === "ask" ? "ask" : "bid");

    const priceSpan = document.createElement("span");
    priceSpan.textContent = price.toFixed(2);

    const quantitySpan = document.createElement("span");
    quantitySpan.textContent = quantity.toFixed(2);

    const totalSpan = document.createElement("span");
    totalSpan.textContent = total.toFixed(2);

    row.appendChild(priceSpan);
    row.appendChild(quantitySpan);
    row.appendChild(totalSpan);

    return row;
}

// Render order book
function renderOrderBook(data) {
    asksContainer.innerHTML = "";
    bidsContainer.innerHTML = "";

    let totalBid = 1;
    // Sort bids in ascending order (worst bid at the top, best bid at the bottom)
    data.bids.sort((a, b) => a.price - b.price);
    data.bids.forEach(order => {
        totalBid = order.price * order.quantity;
        const row = renderOrderRow("bid", order.price, order.quantity, totalBid);
        bidsContainer.appendChild(row);
    });

    let totalAsk = 1;
    // Sort asks in ascending order (best ask at the top)
    data.asks.sort((a, b) => a.price - b.price);
    data.asks.forEach(order => {
        totalAsk = order.price * order.quantity;
        const row = renderOrderRow("ask", order.price, order.quantity, totalAsk);
        asksContainer.appendChild(row);
    });

    renderDepthChart(data);
}

// Render depth chart
function renderDepthChart(data) {
    const askPrices = [];
    const askQuantities = [];
    let cumulativeAsk = 0;

    data.asks.forEach(order => {
        askPrices.push(order.price);
        cumulativeAsk += order.quantity;
        askQuantities.push(cumulativeAsk);
    });

    const bidPrices = [];
    const bidQuantities = [];
    let cumulativeBid = 0;

    data.bids.forEach(order => {
        bidPrices.push(order.price);
        cumulativeBid += order.quantity;
        bidQuantities.push(cumulativeBid);
    });

    const chartData = {
        labels: [...bidPrices.reverse(), ...askPrices],
        datasets: [
            {
                label: "Bids",
                data: bidQuantities.reverse(),
                backgroundColor: "rgba(0, 128, 0, 0.5)",
                borderColor: "rgba(0, 128, 0, 1)",
                fill: true,
            },
            {
                label: "Asks",
                data: askQuantities,
                backgroundColor: "rgba(255, 0, 0, 0.5)",
                borderColor: "rgba(255, 0, 0, 1)",
                fill: true,
            },
        ],
    };

    const chartOptions = {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            x: { title: { display: true, text: "Price" } },
            y: { title: { display: true, text: "Cumulative Quantity" } },
        },
    };

    if (depthChart) depthChart.destroy();
    depthChart = new Chart(ctx, {
        type: "line",
        data: chartData,
        options: chartOptions,
    });
}

// Render trade history
function renderTradeHistory(trades) {
    tradeHistoryList.innerHTML = "";
    trades.forEach(trade => {
        const tradeRow = document.createElement("div");
        tradeRow.classList.add("trade-row");

        const priceSpan = document.createElement("span");
        priceSpan.textContent = trade.price.toFixed(2);

        const quantitySpan = document.createElement("span");
        quantitySpan.textContent = trade.quantity.toFixed(2);

        const timeSpan = document.createElement("span");
        timeSpan.textContent = trade.timestamp;

        tradeRow.appendChild(priceSpan);
        tradeRow.appendChild(quantitySpan);
        tradeRow.appendChild(timeSpan);

        tradeHistoryList.appendChild(tradeRow);
    });
}

// WebSocket event handlers
ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    if (data.trades) renderTradeHistory(data.trades);
    renderOrderBook(data);
};

ws.onerror = (error) => {
    console.error("WebSocket Error: ", error);
};

ws.onclose = () => {
    console.log("WebSocket connection closed.");
};

// Handling the order form submission
orderForm.addEventListener("submit", function (event) {
    event.preventDefault();

    const side = document.getElementById("side").value; // 0 for Buy, 1 for Sell
    const type = document.getElementById("type").value; // 0 for Market, 1 for Limit
    const price = parseFloat(document.getElementById("order-price").value);
    const quantity = parseFloat(document.getElementById("order-quantity").value);

    if (isNaN(price) || isNaN(quantity)) {
        alert("Invalid price or quantity.");
        return;
    }

    // Create the order data in the desired format
    const orderData = {
        Order: {
            price: price,
            quantity: quantity,
            type: parseInt(type), // Market = 0, Limit = 1
            side: parseInt(side), // Buy = 0, Sell = 1
        },
    };

    // Send the order to the server via WebSocket
    ws.send(JSON.stringify(orderData));

    // Clear the form after submission
    orderForm.reset();
});